#pragma once
#include <raylib.h>
#include <raymath.h>
#include "../world/world.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CAMERA_MODE_FREE,     // spectator / debug
    CAMERA_MODE_ORBIT,    // orbit around a target
    CAMERA_MODE_FOLLOW    // follow a moving body
} Kepler_CameraMode;

typedef struct {
    Camera3D camera;
    Kepler_CameraMode mode;

    // Orbit / follow target
    WorldID follow_id;        // body being followed
    Vector3 target;
    float distance;
    float yaw;
    float pitch;

    // Smoothing/Target states (The "Goal" values)
    Vector3 target_goal;
    float distance_goal;
    float yaw_goal;
    float pitch_goal;

    // FREE camera state
    Vector3 velocity;

    // Settings
    float sensitivity;
    float zoom_speed;
    float move_speed;

    World* world;
} CameraState;

static Vector3 camera_forward(const CameraState* cam)
{
    return (Vector3){
        cosf(cam->yaw) * cosf(cam->pitch),
        sinf(cam->pitch),
        sinf(cam->yaw) * cosf(cam->pitch)
    };
}

void camera_init(CameraState* cam, World* world);
void camera_set_follow(CameraState* cam, WorldID id);
void camera_update(CameraState* cam, float dt);

#ifdef __cplusplus
}
#endif
