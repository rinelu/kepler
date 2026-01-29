#pragma once
#include "world_ids.h"
#include <raylib.h>

typedef struct {
    Shader shader;

    float radius;

    bool use_atmosphere;
    bool use_surface;
    bool use_lighting;
    bool emits_light;

    Color base_color;

    float light_intensity;
    float temperature;
    float emissive_strength;
    float roughness;
    float metallic;

    Texture2D surface_texture;
    Texture2D normal_map;

    int loc_mvp;
    int loc_time;
    int loc_radius;
    int loc_color;
} BodyRender;

typedef struct {
    float angle;
} BodyRotation;

typedef struct Body {
    WorldID parent;
    bool visible;

    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;

    float mass;
    float radius;
    float damping;

    BodyRotation rotation;
    BodyRender render;
} Body;

typedef struct {
    Vector3* position;
    Vector3* acceleration;
    float mass;
} GravBodyView;

typedef struct {
    Vector3* position;
    Vector3* velocity;
    Vector3* acceleration;
    float*   mass;
    float*   damping;
} PhysicsBodyView;
