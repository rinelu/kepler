#pragma once
#include <assert.h>
#include <stdbool.h>

// max physics steps per frame
#define TIME_MAX_FRAME_STEPS 32
// seconds of sim time
#define TIME_MAX_ACCUMULATED 1.0f
#define PREDICT_MAX_STEPS 100000

// OCTREE
#define MAX_SUBSTEPS 32
#define ACC_LIMIT    5.0f   // lower = safer, higher = faster
#define NEAR_FIELD_FACTOR 2.5f

#define DENSITY_STAR   1.0f
#define DENSITY_ROCK   3.0f
#define DENSITY_GAS    0.5f

#define VISUAL_RADIUS_SCALE 2

typedef struct {
    float gravity_constant;
    float bh_theta;
    float softening;
    float max_velocity;
    int max_bodies;
} WorldConfig;

typedef struct {
    int  screen_width;
    int  screen_height;
    float time_scale;
    bool vsync;
    bool  paused;
} SimConfig;

typedef struct {
    bool invert_mouse_y;
    bool invert_mouse_x;
    int follow_id;
    int mode;
} CameraConfig;

typedef struct {
    WorldConfig   world;
    SimConfig     sim;
    CameraConfig  camera;
} EngineConfig;

extern EngineConfig g_config;

static inline EngineConfig* config(void)
{
    assert(&g_config);
    return &g_config;
}

bool config_load(const char* path);
