#pragma once

#include "world_ids.h"
#include "orbit.h"
#include "world.h"
#include <raymath.h>

static inline Body create_body(Vector3 position, float radius, Color color)
{
    Body body = {0};

    body.visible = true;
    body.position = position;
    body.radius = radius;

    body.render.base_color = color;
    body.render.radius = radius;

    body.render.loc_mvp = -1;
    body.render.loc_time = -1;
    body.render.loc_radius = -1;
    body.render.loc_color = -1;
    body.render.loc_light_dir = -1;

    return body;
}

static inline WorldID spawn_body(World* world, Vector3 position, float radius, Color color)
{
    Body body = create_body(position, radius, color);
    return world_add_body(world, &body);
}

static inline WorldID spawn_body_with_shader(World* world, Vector3 position, float radius, Color color, const char* shader_path)
{
    Body body = create_body(position, radius, color);
    body.render.shader = LoadShader(0, shader_path);
    return world_add_body(world, &body);
} 
