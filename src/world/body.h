#pragma once
#include "world_ids.h"
#include <raylib.h>

typedef struct {
    Shader shader;

    float radius;

    bool use_atmosphere;
    bool use_surface;
    bool use_lighting;

    Color base_color;
    float roughness;
    float metallic;

    Texture2D surface_texture;
    Texture2D normal_map;

    int loc_mvp;
    int loc_time;
    int loc_radius;
    int loc_color;
    int loc_light_dir;

} BodyRender;

typedef struct {
    float angle;
} BodyRotation;

typedef struct Body {
    WorldID parent;
    bool visible;

    Vector3 local_position;
    Vector3 local_velocity;
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Vector3 prev_acceleration;

    float   mass;
    float   radius;

    BodyRotation rotation;
    BodyRender render;
} Body;
