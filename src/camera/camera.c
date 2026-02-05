#include "camera.h"
#include "core/config.h"
#include "gui/gui_layer.h"
#include <string.h>
#include <math.h>

static float wrap_angle(float a)
{
    while (a >  PI) a -= 2.0f * PI;
    while (a < -PI) a += 2.0f * PI;
    return a;
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

static void camera_update_free(CameraState *cam, float dt)
{
    if (!ImGuiLayer_CaptureInput() && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 d = GetMouseDelta();
        cam->yaw += d.x * cam->sensitivity;
        cam->pitch += d.y * cam->sensitivity;
    }
    cam->pitch      = Clamp(cam->pitch, -PI * 0.49f, PI * 0.49f);
    Vector3 forward = camera_forward(cam);
    Vector3 right   = Vector3Normalize(Vector3CrossProduct(forward, cam->camera.up));
    Vector3 move    = {0};

    if (!ImGuiLayer_CaptureInput()) {
        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_E)) move.y += 1.0f;
        if (IsKeyDown(KEY_Q)) move.y -= 1.0f;
    }

    float len = Vector3Length(move);
    if (len > 0.0001f) {
        move = Vector3Scale(move, 1.0f / len);
        cam->camera.position = Vector3Add(cam->camera.position, Vector3Scale(move, cam->move_speed * dt));
    }
    cam->camera.target = Vector3Add(cam->camera.position, forward);
}

static void camera_update_orbit(CameraState* cam, float dt)
{
    if (!ImGuiLayer_CaptureInput() && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 d = GetMouseDelta();

        float inv_x = config()->camera.invert_mouse_x ? -1.0f : 1.0f;
        float inv_y = config()->camera.invert_mouse_y ? -1.0f : 1.0f;

        cam->yaw_goal   += d.x * cam->sensitivity * inv_x;
        cam->pitch_goal += d.y * cam->sensitivity * inv_y;
    }

    cam->pitch_goal = Clamp(cam->pitch_goal, -PI * 0.49f, PI * 0.49f);

    if (!ImGuiLayer_CaptureInput()) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            float zoom_factor = powf(1.1f, -wheel);
            cam->distance_goal *= zoom_factor;
            cam->distance_goal = Clamp(cam->distance_goal, 0.01f, 1e9f);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 d = GetMouseDelta();
            Vector3 view  = Vector3Subtract(cam->target, cam->camera.position);
            Vector3 right = Vector3Normalize(Vector3CrossProduct(view, cam->camera.up));
            Vector3 up    = Vector3Normalize(Vector3CrossProduct(right, view));
            float pan = fmaxf(cam->distance, 0.5f) * 0.002f;

            cam->target_goal = Vector3Add(cam->target_goal, Vector3Add(Vector3Scale(right, -d.x * pan), Vector3Scale(up, d.y * pan)));
        }
    }
    float s = Clamp(12.0f * dt, 0.0f, 1.0f);
    cam->yaw      += wrap_angle(cam->yaw_goal - cam->yaw) * s;
    cam->pitch    += (cam->pitch_goal - cam->pitch) * s;
    cam->distance += (cam->distance_goal - cam->distance) * s;
    cam->target    = Vector3Lerp(cam->target, cam->target_goal, s);

    Vector3 forward = camera_forward(cam);
    cam->camera.position = Vector3Subtract(cam->target, Vector3Scale(forward, cam->distance));
    cam->camera.target = cam->target;
}

static void camera_update_follow(CameraState* cam, float dt)
{
    Body* body = world_get_body(cam->world, cam->follow_id);
    if (!body) return;

    cam->target_goal = body->position;
    camera_update_orbit(cam, dt);
}

void camera_update(CameraState* cam, float dt)
{
    switch (cam->mode) {
        case CAMERA_MODE_FREE: camera_update_free(cam, dt); break;
        case CAMERA_MODE_ORBIT:  camera_update_orbit(cam, dt);  break;
        case CAMERA_MODE_FOLLOW: camera_update_follow(cam, dt); break;
        default: break;
    }
}
