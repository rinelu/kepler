#pragma once
#include "../app/app.h"

#define BH_THETA 0.6f
#define SOFTENING 0.01f

typedef struct OctreeNode {
    Vector3 center;
    float half_size;

    float mass;
    Vector3 com;

    Body* body; // leaf only
    struct OctreeNode* children[8];
} OctreeNode;

void octree_update(App* app, float dt);
