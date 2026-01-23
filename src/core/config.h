#pragma once
#include <assert.h>
#include <stdbool.h>

typedef struct {
    float gravity_constant;
    float bh_theta;
    float softening;
    float max_velocity;
    int max_bodies;
} WorldConfig;

typedef struct {
    bool  enabled;
    float damping_default;
} PhysicsConfig;

typedef struct {
    int  screen_width;
    int  screen_height;
    float time_scale;
    bool vsync;
    bool  paused;
} SimConfig;

typedef struct {
    WorldConfig   world;
    PhysicsConfig physics;
    SimConfig     sim;
} EngineConfig;

extern EngineConfig g_config;

static const EngineConfig* config(void)
{
    assert(&g_config);
    return &g_config;
}

bool config_load(const char* path);
