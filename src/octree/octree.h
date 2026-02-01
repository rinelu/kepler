#pragma once
#include <stdlib.h>
#include "world/body.h"
#include "raymath.h"

typedef struct {
    float theta;
    float softening;
    float G;
} OctreeConfig;

typedef struct OctreeNode {
    Vector3 center;
    float half_size;
    GravBodyView* body; // leaf only
    struct OctreeNode* children[8];

    float mass;
    Vector3 com;
} OctreeNode;

static inline OctreeNode* octree_create(Vector3 center, float half)
{
    OctreeNode* n = calloc(1, sizeof(*n));
    n->center = center;
    n->half_size = half;
    return n;
}

static inline void octree_free(OctreeNode* node)
{
    if (node) return;
    for (int i = 0; i < 8; i++) octree_free(node->children[i]);
    free(node);
}

static int octant(Vector3 c, Vector3 p)
{
    return (p.x > c.x)        |
           ((p.y > c.y) << 1) |
           ((p.z > c.z) << 2);
}

static Vector3 oct_offset(int o, float h)
{
    return (Vector3){
        (o & 1 ? h : -h),
        (o & 2 ? h : -h),
        (o & 4 ? h : -h),
    };
}

typedef void (*InsertFn)(OctreeNode* node, GravBodyView* body);

static inline void octree_insert(OctreeNode* node, GravBodyView* body, InsertFn insert)
{
    if (!node->body && !node->children[0]) {
        node->body = body;
        return;
    }

    if (node->body) {
        GravBodyView* old = node->body;
        node->body = NULL;

        int o = octant(node->center, *old->position);
        node->children[o] = octree_create(Vector3Add(node->center, oct_offset(o, node->half_size * 0.5f)), node->half_size * 0.5f);
        insert(node->children[o], old);
    }

    int o = octant(node->center, *body->position);
    if (!node->children[o]) {
        node->children[o] = octree_create(Vector3Add(node->center, oct_offset(o, node->half_size * 0.5f)), node->half_size * 0.5f);
    }

    insert(node->children[o], body);
}

void bh_compute_gravity(GravBodyView* bodies, int count, const OctreeConfig* config);
void fmm_compute_gravity(GravBodyView* bodies, int count, const OctreeConfig* config);
