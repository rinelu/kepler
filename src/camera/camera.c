#include "camera.h"
#include <string.h>
#include <math.h>

void camera_init(CameraState* cam, World* world)
{
    memset(cam, 0, sizeof(*cam));

    cam->mode = CAMERA_MODE_ORBIT;

    cam->target = cam->target_goal = (Vector3){ 0, 0, 0 };

    cam->distance = cam->distance_goal = 20.0f;
    cam->yaw = cam->yaw_goal = PI / 4.0f;
    cam->pitch = cam->pitch_goal = 0.3f;

    cam->sensitivity = 0.003f;
    cam->zoom_speed  = 0.15f;
    cam->move_speed  = 25.0f;

    cam->camera.up = (Vector3){ 0, 1, 0 };
    cam->camera.fovy = 45.0f;
    cam->camera.projection = CAMERA_PERSPECTIVE;

    cam->world = world;
}

void camera_set_follow(CameraState* cam, WorldID id)
{
    cam->mode = CAMERA_MODE_FOLLOW;
    cam->follow_id = id;

    Body* body = world_get_body(cam->world, id);
    if (!body) return;

    cam->target        =
    cam->target_goal   = body->position;
    cam->distance      =
    cam->distance_goal = body->radius * 6.0f;
    cam->yaw           =
    cam->yaw_goal      = 0.0f;
    cam->pitch         =
    cam->pitch_goal    = 0.25f;

    float h = cam->distance * cosf(cam->pitch);

    cam->camera.position = (Vector3){
        cam->target.x + sinf(cam->yaw) * h,
        cam->target.y + cam->distance * sinf(cam->pitch),
        cam->target.z + cosf(cam->yaw) * h
    };

    cam->camera.target = cam->target;
}

static float wrap_angle(float angle)
{
    while (angle >  PI) angle -= 2.0f * PI;
    while (angle < -PI) angle += 2.0f * PI;
    return angle;
}

static Vector3 camera_forward(CameraState* cam)
{
    return Vector3Normalize(Vector3Subtract(cam->camera.target, cam->camera.position));
}

static void camera_update_free(CameraState* cam, float dt)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 d = GetMouseDelta();
        cam->yaw   += d.x * cam->sensitivity;
        cam->pitch += d.y * cam->sensitivity;
    }

    cam->pitch = Clamp(cam->pitch, -PI * 0.49f, PI * 0.49f);

    Vector3 forward = {
        sinf(cam->yaw) * cosf(cam->pitch),
        sinf(cam->pitch),
        cosf(cam->yaw) * cosf(cam->pitch)
    };

    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, cam->camera.up));
    Vector3 move = {0};

    if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
    if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
    if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
    if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);
    if (IsKeyDown(KEY_E)) move.y += 1.0f;
    if (IsKeyDown(KEY_Q)) move.y -= 1.0f;

    cam->camera.position = Vector3Add(cam->camera.position,
                                      Vector3Scale(Vector3Normalize(move), cam->move_speed * dt));
    cam->camera.target   = Vector3Add(cam->camera.position, forward);
}

static void camera_update_orbit(CameraState* cam, float dt)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 d = GetMouseDelta();
        cam->yaw_goal   += d.x * cam->sensitivity;
        cam->pitch_goal += d.y * cam->sensitivity;
    }

    cam->pitch_goal = Clamp(cam->pitch_goal, -PI * 0.48f, PI * 0.48f);

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        cam->distance_goal -= wheel * (cam->distance * cam->zoom_speed);
        cam->distance_goal = fmaxf(cam->distance_goal, 0.1f);
    }

    // Pan
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 d     = GetMouseDelta();
        Vector3 view  = Vector3Subtract(cam->target, cam->camera.position);
        Vector3 right = Vector3Normalize(Vector3CrossProduct(view, cam->camera.up));
        Vector3 up    = Vector3Normalize(Vector3CrossProduct(right, view));

        float pan = cam->distance * 0.0012f;
        cam->target_goal = Vector3Add(cam->target_goal,
                                      Vector3Add(Vector3Scale(right, -d.x * pan), Vector3Scale(up, d.y * pan)));
    }

    float s = Clamp(12.0f * dt, 0, 1);

    cam->yaw += wrap_angle(cam->yaw_goal - cam->yaw) * s;
    cam->pitch    = Lerp(cam->pitch, cam->pitch_goal, s);
    cam->distance = Lerp(cam->distance, cam->distance_goal, s);
    cam->target   = Vector3Lerp(cam->target, cam->target_goal, s);

    float h = cam->distance * cosf(cam->pitch);
    float v = cam->distance * sinf(cam->pitch);

    cam->camera.position = (Vector3){
        cam->target.x + sinf(cam->yaw) * h,
        cam->target.y + v,
        cam->target.z + cosf(cam->yaw) * h
    };

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
        case CAMERA_MODE_FREE:   camera_update_free(cam, dt);   break;
        case CAMERA_MODE_ORBIT:  camera_update_orbit(cam, dt);  break;
        case CAMERA_MODE_FOLLOW: camera_update_follow(cam, dt); break;
    }
}

