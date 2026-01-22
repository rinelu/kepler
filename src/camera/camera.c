#include "camera.h"
#include <math.h>

void camera_init(CameraState* cam, World* world)
{
    cam->target_goal = (Vector3){ 0, 0, 0 };
    cam->target = cam->target_goal;

    cam->distance_goal = 20.0f;
    cam->distance = cam->distance_goal;

    cam->yaw_goal = PI / 4.0f;
    cam->yaw = cam->yaw_goal;

    cam->pitch_goal = 0.3f; // Positive looks down
    cam->pitch = cam->pitch_goal;

    cam->sensitivity = 0.003f;
    cam->zoom_speed = 0.1f;

    cam->camera.up = (Vector3){ 0, 1, 0 };
    cam->camera.fovy = 45.0f;
    cam->camera.projection = CAMERA_PERSPECTIVE;

    cam->world = world;
}

static float wrap_angle(float angle)
{
    while (angle >  PI) angle -= 2.0f * PI;
    while (angle < -PI) angle += 2.0f * PI;
    return angle;
}

void camera_update(CameraState* cam, float dt)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        // Sensitivity feels better when adjusted by FOV
        float sensitivity = cam->sensitivity * (cam->camera.fovy / 60.0f);
        cam->yaw_goal   += delta.x * sensitivity;
        cam->pitch_goal += delta.y * sensitivity;
    }

    // Clamp Pitch to avoid flipping at poles (90 degrees up/down)
    // We use 0.48 to leave a tiny gap so the "Up" vector doesn't glitch
    cam->pitch_goal = Clamp(cam->pitch_goal, -PI * 0.48f, PI * 0.48f);

    // Zoom logic
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        cam->distance_goal -= wheel * (cam->distance * 0.15f);
        if (cam->distance_goal < 0.1f) cam->distance_goal = 0.1f;
    }

    // Panning logic: Move relative to camera view
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = GetMouseDelta();

        // Calculate the view direction
        Vector3 viewDir = Vector3Subtract(cam->target, cam->camera.position);
        Vector3 right = Vector3Normalize(Vector3CrossProduct(viewDir, cam->camera.up));
        Vector3 up = Vector3Normalize(Vector3CrossProduct(right, viewDir));

        float panSpeed = cam->distance * 0.0012f;
        cam->target_goal = Vector3Add(cam->target_goal, Vector3Scale(right, -delta.x * panSpeed));
        cam->target_goal = Vector3Add(cam->target_goal, Vector3Scale(up, delta.y * panSpeed));
    }

    // SMOOTHING
    float speed = 12.0f * dt; // Adjust 12.0 for more/less "weight"
    if (speed > 1.0f) speed = 1.0f;

    float yawDiff = wrap_angle(cam->yaw_goal - cam->yaw);
    cam->yaw += yawDiff * speed;

    cam->pitch    = Lerp(cam->pitch, cam->pitch_goal, speed);
    cam->distance = Lerp(cam->distance, cam->distance_goal, speed);
    cam->target   = Vector3Lerp(cam->target, cam->target_goal, speed);

    // Perfect Orbit Math
    float horizontalDist = cam->distance * cosf(cam->pitch);
    float verticalDist   = cam->distance * sinf(cam->pitch);

    Vector3 pos;
    pos.x = cam->target.x + sinf(cam->yaw) * horizontalDist;
    pos.y = cam->target.y + verticalDist;
    pos.z = cam->target.z + cosf(cam->yaw) * horizontalDist;

    cam->camera.position = pos;
    cam->camera.target = cam->target;
}
