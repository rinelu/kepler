#pragma once
#include "world/body.h"

typedef struct OctreeConfig {
    float theta;
    float softening;
    float G;
} OctreeConfig;

void octree_compute_gravity(GravBodyView* bodies, int count, const OctreeConfig* config);
