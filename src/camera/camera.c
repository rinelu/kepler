#include "camera.h"
#include "core/config.h"
#include "gui/gui_layer.h"
#include <string.h>
#include <math.h>
#include "raymath.h"

static inline float wrap_angle(float a)
{
    if (a >  PI) a -= 2.0f * PI;
    if (a < -PI) a += 2.0f * PI;
    return a;
}

static inline void camera_update_basis(CameraState* cam)
{
    float cy = cosf(cam->yaw);
    float sy = sinf(cam->yaw);
    float cp = cosf(cam->pitch);
    float sp = sinf(cam->pitch);

    cam->forward = (Vector3){ cy * cp, sp, sy * cp };
    cam->right = (Vector3){ -sy, 0.0f, cy };
    cam->up = Vector3CrossProduct(cam->right, cam->forward);
}

static inline void camera_handle_mouse_look(CameraState* cam, bool capture, bool smooth)
{
    if (capture || !IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        return;

    Vector2 d = GetMouseDelta();

    float inv_x = config()->camera.invert_mouse_x ? -1.0f : 1.0f;
    float inv_y = config()->camera.invert_mouse_y ? -1.0f : 1.0f;

    float dx = d.x * cam->sensitivity * inv_x;
    float dy = d.y * cam->sensitivity * inv_y;

    if (smooth) {
        cam->yaw_goal   += dx;
        cam->pitch_goal += dy;
        return;
    }
    cam->yaw   += dx;
    cam->pitch += dy;
}

static void camera_update_free(CameraState* cam, float dt)
{
    bool capture = ImGuiLayer_CaptureInput();
    camera_handle_mouse_look(cam, capture, false);
    cam->pitch = Clamp(cam->pitch, -PI * 0.49f, PI * 0.49f);
    camera_update_basis(cam);

    Vector3 move = {0};
    if (!capture) {
        if (IsKeyDown(KEY_W)) move = Vector3Add(move, cam->forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, cam->forward);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, cam->right);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, cam->right);
        if (IsKeyDown(KEY_E)) move.y += 1.0f;
        if (IsKeyDown(KEY_Q)) move.y -= 1.0f;
    }

    float len2 = Vector3LengthSqr(move);
    if (len2 > 0.0001f) {
        move = Vector3Scale(move, cam->move_speed * dt / sqrtf(len2));
        cam->camera.position = Vector3Add(cam->camera.position, move);
    }

    cam->camera.target = Vector3Add(cam->camera.position, cam->forward);
}

static void camera_update_orbit(CameraState* cam, float dt)
{
    bool capture = ImGuiLayer_CaptureInput();
    camera_handle_mouse_look(cam, capture, true);
    cam->pitch_goal = Clamp(cam->pitch_goal, -PI * 0.49f, PI * 0.49f);

    if (!capture) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            cam->distance_goal *= powf(1.1f, -wheel);
            cam->distance_goal = Clamp(cam->distance_goal, 0.01f, 1e9f);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 d = GetMouseDelta();
            float pan = fmaxf(cam->distance, 0.5f) * 0.002f;

            cam->target_goal = Vector3Add(cam->target_goal, Vector3Add(Vector3Scale(cam->right, -d.x * pan), Vector3Scale(cam->up, d.y * pan)));
        }
    }

    float s = Clamp(dt * 12.0f, 0.0f, 1.0f);

    cam->yaw      += wrap_angle(cam->yaw_goal - cam->yaw) * s;
    cam->pitch    += (cam->pitch_goal - cam->pitch) * s;
    cam->distance += (cam->distance_goal - cam->distance) * s;
    cam->target    = Vector3Lerp(cam->target, cam->target_goal, s);

    camera_update_basis(cam);

    cam->camera.position = Vector3Subtract(cam->target, Vector3Scale(cam->forward, cam->distance));
    cam->camera.target = cam->target;
}

static void camera_update_follow(CameraState* cam, float dt)
{
    Body* body = world_get_body(cam->world, cam->follow_id);
    if (!body) return;

    cam->target_goal = body->position;
    camera_update_orbit(cam, dt);
}

void camera_init(CameraState* cam, World* world)
{
    memset(cam, 0, sizeof(*cam));

    cam->mode = CAMERA_MODE_ORBIT;

    cam->target = cam->target_goal = (Vector3){ 0, 0, 0 };

    cam->distance = cam->distance_goal = 20.0f;
    cam->yaw = cam->yaw_goal = PI * 0.25f;
    cam->pitch = cam->pitch_goal = 0.3f;

    cam->sensitivity = 0.003f;
    cam->move_speed = 10.0f;

    cam->camera.up = (Vector3){ 0, 1, 0 };
    cam->camera.fovy = 45.0f;
    cam->camera.projection = CAMERA_PERSPECTIVE;

    cam->world = world;
}

void camera_set_follow(CameraState* cam, WorldID id)
{
    Body* body = world_get_body(cam->world, id);
    if (!body) return;

    cam->mode = CAMERA_MODE_FOLLOW;
    cam->follow_id = id;

    cam->target = cam->target_goal = body->position;
    cam->distance = cam->distance_goal = body->radius * 6.0f;

    cam->yaw = cam->yaw_goal = 0.0f;
    cam->pitch = cam->pitch_goal = 0.25f;
}

void camera_update(CameraState* cam, float dt)
{
    switch (cam->mode) {
        case CAMERA_MODE_FREE:   camera_update_free(cam, dt); break;
        case CAMERA_MODE_ORBIT:  camera_update_orbit(cam, dt);  break;
        case CAMERA_MODE_FOLLOW: camera_update_follow(cam, dt); break;
        default: break;
    }
}
