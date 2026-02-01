#pragma once

#include "core/log.h"
#include "world.h"
#include "world_ids.h"
#include <raymath.h>
#include <math.h>

#define DENSITY_STAR   1.0f
#define DENSITY_ROCK   3.0f
#define DENSITY_GAS    0.5f

#define VISUAL_RADIUS_SCALE 2

typedef struct {
    const char* name;
    float   mass;
    float   density;

    Vector3 position;
    Vector3 velocity;

    Color   base_color;   // fallback applied if alpha == 0
} BodyParam;

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

static inline Body create_physical_body(const BodyParam* param)
{
    Body b = {0};

    b.parent  = WORLD_ID_INVALID;
    b.visible = true;

    b.mass         = param->mass;
    b.damping      = 0.05f;
    b.acceleration = Vector3Zero();

    b.position = param->position;
    b.velocity = param->velocity;

    float physical_radius = compute_radius_from_mass(param->mass, param->density);
    b.radius = physical_radius;

    b.render.radius     = physical_radius * VISUAL_RADIUS_SCALE;
    b.render.base_color = (param->base_color.a == 0) ? WHITE : param->base_color;

    b.render.use_atmosphere  = false;
    b.render.emits_light     = false;
    b.render.light_intensity = 0.0f;
    b.render.temperature     = 0;
    b.render.emissive_strength = 0.3f;

    return b;
}

static inline WorldID spawn_body(World* world, const BodyParam* param)
{
    Body body = create_physical_body(param);
    return world_add_body(world, &body);
}

static inline void body_make_star(Body* b, float intensity, float emissive, float temperature)
{
    b->render.emits_light = true;
    b->render.light_intensity = intensity;
    b->render.emissive_strength = emissive;
    b->render.temperature = temperature;
    LOG_TODO("Star temperature");
}

static inline WorldID spawn_body_with_shader(World* world, const BodyParam* desc, const SpawnBodyShader* shader)
{
    LOG_TODO("Each body shader.");
    Body body = create_physical_body(desc);
    // body.render.shader = LoadShader(shader->vertex, shader->fragment);
    return world_add_body(world, &body);
}

static inline void body_set_damping(Body* b, float damping)
{
    b->damping = Clamp(damping, 0.0f, 0.01f);
}
