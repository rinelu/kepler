#pragma once
#include "world_ids.h"
#include <raylib.h>

#define TRAIL_MAX_POINTS 256

typedef struct {
    Vector3 points[TRAIL_MAX_POINTS];
    int head;
    int count;
} Trail;

typedef struct {
    Shader shader;

    float radius;

    bool use_atmosphere;
    bool use_surface;
    bool use_lighting;
    bool emits_light;

    Color base_color;

    float light_intensity;
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

    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;

    float mass;
    float radius;
    float damping;

    BodyRotation rotation;
    BodyRender render;
    Trail trail;
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

static void trail_push(Body* b)
{
    b->trail.points[b->trail.head] = b->position;
    b->trail.head = (b->trail.head + 1) % TRAIL_MAX_POINTS;

    if (b->trail.count < TRAIL_MAX_POINTS) b->trail.count++;
}
