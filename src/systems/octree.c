#include "octree.h"
#include <iso646.h>
#include <stdlib.h>
#include <math.h>
#include "raymath.h"

typedef struct OctreeNode {
    Vector3 center;
    float half_size;

    float mass;
    Vector3 com;

    GravBodyView* body; // leaf only
    int body_count;
    struct OctreeNode* children[8];
} OctreeNode;

static OctreeNode* octree_create(Vector3 center, float half)
{
    OctreeNode* n = calloc(1, sizeof(*n));
    n->center = center;
    n->half_size = half;
    return n;
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

static void octree_insert(OctreeNode* node, GravBodyView* gb)
{
    node->body_count++;
    Vector3 pos = *gb->position;
    float mass  = gb->mass;

    if (!node->body && !node->children[0]) {
        node->body = gb;
        node->mass = mass;
        node->com  = pos;
        return;
    }

    if (node->body) {
        GravBodyView* old = node->body;
        node->body = NULL;

        Vector3 old_pos = *old->position;
        int o = octant(node->center, old_pos);

        node->children[o] = octree_create(Vector3Add(node->center, oct_offset(o, node->half_size * 0.5f)), node->half_size * 0.5f);

        octree_insert(node->children[o], old);
    }

    int o = octant(node->center, pos);
    if (!node->children[o]) {
        node->children[o] = octree_create(Vector3Add(node->center, oct_offset(o, node->half_size * 0.5f)), node->half_size * 0.5f);
    }

    octree_insert(node->children[o], gb);

    node->mass = 0.0f;
    node->com  = (Vector3){0};

    for (int i = 0; i < 8; i++) {
        if (node->children[i]) {
            float child_mass = node->children[i]->mass;
            if (child_mass > 0.0f) {
                node->com = Vector3Add(node->com, Vector3Scale(node->children[i]->com, child_mass));
                node->mass += child_mass;
            }
        }
    }

    if (node->body_count > 1 && node->mass > 0.0f) {
        node->com = Vector3Scale(node->com, 1.0f / node->mass);
    }
}

static void octree_apply_force(const OctreeNode* node, GravBodyView* bodies, int i, const OctreeConfig* cfg)
{
    if (!node || node->mass == 0.0f) return;
    if (node->body_count == 1 && node->body == &bodies[i]) return;

    Vector3 d = Vector3Subtract(node->com, *bodies[i].position);
    float dist2 = Vector3LengthSqr(d) + cfg->softening * cfg->softening;
    float dist = sqrtf(dist2);

    if (node->body_count == 1 || (node->half_size / dist) < cfg->theta) {
        float inv_r3 = 1.0f / (dist2 * dist);
        Vector3 a = Vector3Scale(d, cfg->G * node->mass * inv_r3);
        *bodies[i].acceleration = Vector3Add(*bodies[i].acceleration, a);
        return;
    }

    for (int k = 0; k < 8; k++)
        octree_apply_force(node->children[k], bodies, i, cfg);
}

static void octree_free(OctreeNode* node)
{
    if (!node) return;
    for (int i = 0; i < 8; i++) octree_free(node->children[i]);
    free(node);
}

void octree_compute_gravity(GravBodyView* bodies, int count, const OctreeConfig* cfg)
{
    if (count == 0) return;


    Vector3 min = *bodies[0].position;
    Vector3 max = *bodies[0].position;

    for (int i = 1; i < count; i++) {
        min = Vector3Min(min, *bodies[i].position);
        max = Vector3Max(max, *bodies[i].position);
    }

    Vector3 center = Vector3Scale(Vector3Add(min, max), 0.5f);
    float half_size = 0.5f * fmaxf(fmaxf(max.x - min.x, max.y - min.y), max.z - min.z) + 1e-3f;

    OctreeNode* root = octree_create(center, half_size);

    for (int i = 0; i < count; i++) octree_insert(root, &bodies[i]);
    for (int i = 0; i < count; i++) octree_apply_force(root, bodies, i, cfg);

    octree_free(root);
}

