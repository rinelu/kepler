#include "octree_system.h"
#include <sched.h>
#include <stdlib.h>

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

static void octree_insert(OctreeNode* node, Body* body)
{
    if (!node->body && !node->children[0]) {
        node->body = body;
        node->mass = body->mass;
        node->com = body->position;
        return;
    }

    if (node->body) {
        Body* old = node->body;
        node->body = NULL;

        int o = octant(node->center, old->position);
        node->children[o] = octree_create(
            Vector3Add(node->center, oct_offset(o, node->half_size * 0.5f)),
            node->half_size * 0.5f);
        octree_insert(node->children[o], old);
    }

    int o = octant(node->center, body->position);
    if (!node->children[o]) {
        node->children[o] = octree_create(
            Vector3Add(node->center, oct_offset(o, node->half_size * 0.5f)),
            node->half_size * 0.5f);
    }

    octree_insert(node->children[o], body);

    node->mass = 0;
    node->com = (Vector3){0};

    for (int i = 0; i < 8; i++) {
        if (node->children[i] && node->children[i]->mass > 0) {
            node->mass += node->children[i]->mass;
            node->com = Vector3Add(
                node->com,
                Vector3Scale(node->children[i]->com, node->children[i]->mass));
        }
    }

    node->com = Vector3Scale(node->com, 1.0f / node->mass);
}


static void octree_apply(OctreeNode* node, Body* target, float G)
{
    if (!node || node->mass <= 0.0f)
        return;

    if (node->body == target)
        return;

    Vector3 d = Vector3Subtract(node->com, target->position);
    float soft = config()->world.softening;
    float dist2 = Vector3LengthSqr(d) + soft * soft;
    float dist  = sqrtf(dist2);

    float size = node->half_size * 2.0f;

    if (node->body != NULL || (size / dist) < config()->world.bh_theta) {
        Vector3 accel = Vector3Scale(d, G * node->mass / (dist2 * dist));
        target->acceleration = Vector3Add(target->acceleration, accel);
        return;
    }

    // Otherwise recurse
    for (int i = 0; i < 8; i++)
        if (node->children[i])
            octree_apply(node->children[i], target, G);
}


static void octree_integrate_pre(World* world, float dt)
{
    Body* b;
    world_foreach_body(world, b) {
        b->velocity = Vector3Add(b->velocity, Vector3Scale(b->prev_acceleration, 0.5f * dt));

        if (b->damping > 0.0f) {
            b->velocity = Vector3Scale(b->velocity, 1.0f - b->damping);
        }

        b->position = Vector3Add(b->position, Vector3Scale(b->velocity, dt));
    }
}

static void octree_integrate_post(World* world, float dt)
{
    Body* b;
    world_foreach_body(world, b) {
        b->velocity = Vector3Add(b->velocity, Vector3Scale(b->acceleration, 0.5f * dt));
    }
}

static void octree_free(OctreeNode* node)
{
    if (!node) return;

    for (int i = 0; i < 8; i++)
        if (node->children[i])
            octree_free(node->children[i]);

    free(node);
}

void octree_update(App* app, float dt)
{
    World* world = app->services.world;
    float G = config()->world.gravity_constant;

    // Store previous acceleration
    Body* b;
    world_foreach_body(world, b) {
        b->prev_acceleration = b->acceleration;
        b->acceleration = (Vector3){0};
    }

    octree_integrate_pre(world, dt);

    Vector3 center = {0};
    int count = 0;
    world_foreach_body(world, b) {
        center = Vector3Add(center, b->position);
        count++;
    }
    center = Vector3Scale(center, 1.0f / count);
    // TODO: add variable to config
    OctreeNode* root = octree_create(center, 5000);

    world_foreach_body(world, b) if (b->mass > 0) octree_insert(root, b);
    world_foreach_body(world, b) if (b->mass > 0) octree_apply(root, b, G);

    octree_integrate_post(world, dt);
    world_foreach_body(world, b) trail_push(b);

    octree_free(root);
}
