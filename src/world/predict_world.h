#pragma once
#include "world/world.h"
#include <raylib.h>

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    float   mass;
    float   damping;
} PredictBody;

typedef struct {
    PredictBody* bodies;
    int count;
} PredictWorld;

void predict_world_clone(const World* src, PredictWorld* dst);
void predict_build_physics_views(PredictWorld* pw, GravBodyView* grav, PhysicsBodyView* phys);
void predict_world_free(PredictWorld* pw);
