#pragma once
#include "world_ids.h"
#include <raylib.h>

typedef struct {
    Mesh  sphere_mesh[3];
    Model sphere_model[3];
} BodyMesh;

typedef struct {
    Shader shader;

    int loc_mvp;
    int loc_model;
    int loc_color;
    int loc_light;
    int loc_emissive;
    int loc_light_count;
    int loc_light_pos;
    int loc_light_color;
    int loc_light_intensity;
} BodyShader;

typedef struct {
    BodyShader* shader;
    BodyMesh* mesh;

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
