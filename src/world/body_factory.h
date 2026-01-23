#pragma once

#include "world.h"
#include <raymath.h>
#include <math.h>

#define DENSITY_STAR   1.0f
#define DENSITY_ROCK   3.0f
#define DENSITY_GAS    0.5f

#define VISUAL_RADIUS_SCALE 1.5f

typedef struct {
    const char* name;
    float   mass;
    float   density;

    Vector3 position;
    Vector3 velocity;

    Color   base_color;   // optional, fallback applied if zero
} SpawnBodyDesc;

typedef struct {
    const char* vertex;
    const char* fragment;
} SpawnBodyShader;

static inline float compute_radius_from_mass(float mass, float density)
{
    if (mass <= 0.0f || density <= 0.0f) return 0.0f;

    float volume = mass / density;
    return cbrtf((3.0f * volume) / (4.0f * PI));
}

static inline Body create_physical_body(const SpawnBodyDesc* desc)
{
    Body b = {0};

    b.visible = true;
    b.mass    = desc->mass;
    b.damping = 0.0f;

    b.position = desc->position;
    b.velocity = desc->velocity;

    float physical_radius = compute_radius_from_mass(desc->mass, desc->density);
    b.radius = physical_radius;

    b.trail.head  = 0;
    b.trail.count = 0;

    b.render.radius     = physical_radius * VISUAL_RADIUS_SCALE;
    b.render.base_color = (desc->base_color.a == 0) ? WHITE : desc->base_color;

    b.render.loc_mvp       = -1;
    b.render.loc_time      = -1;
    b.render.loc_radius    = -1;
    b.render.loc_color     = -1;
    b.render.loc_light_dir = -1;

    return b;
}

static inline WorldID spawn_body(World* world, const SpawnBodyDesc* desc)
{
    Body body = create_physical_body(desc);
    return world_add_body(world, &body);
}

static inline WorldID spawn_body_with_shader(World* world, const SpawnBodyDesc* desc, SpawnBodyShader* shader)
{
    Body body = create_physical_body(desc);
    body.render.shader = LoadShader(shader->vertex, shader->fragment);
    return world_add_body(world, &body);
}

static inline void body_set_damping(Body* b, float damping)
{
    b->damping = Clamp(damping, 0.0f, 0.01f);
}
