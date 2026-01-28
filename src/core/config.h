#pragma once
#include <assert.h>
#include <stdbool.h>

#define PREDICT_MAX_STEPS 100000
// OCTREE
#define NEAR_FIELD_FACTOR 2.5f

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
    WorldConfig   world;
    SimConfig     sim;
} EngineConfig;

extern EngineConfig g_config;

static inline EngineConfig* config(void)
{
    assert(&g_config);
    return &g_config;
}

bool config_load(const char* path);
