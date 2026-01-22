#include "octree_system.h"

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
    if (!node || node->mass <= 0 || node->body == target)
        return;

    Vector3 d = Vector3Subtract(node->com, target->position);
    float dist2 = Vector3LengthSqr(d) + SOFTENING;
    float dist = sqrtf(dist2);

    float size = node->half_size * 2.0f;

    if (!node->children[0] || (size / dist) < BH_THETA) {
        Vector3 accel = Vector3Scale(d, G * node->mass / (dist2 * dist));
        target->acceleration = Vector3Add(target->acceleration, accel);
        return;
    }

    for (int i = 0; i < 8; i++)
        octree_apply(node->children[i], target, G);
}

static void octree_integrate(World* world, float dt)
{
    Body* b;
    world_foreach_body(world, b) {
        b->velocity = Vector3Add(
            b->velocity,
            Vector3Scale(b->acceleration, dt)
        );
        b->position = Vector3Add(
            b->position,
            Vector3Scale(b->velocity, dt)
        );
    }
}

void octree_update(App* app, float dt)
{
    World* world = app->services.world;
    float G = app->config.world.gravity_constant;

    // Clear acceleration
    Body* b;
    world_foreach_body(world, b) b->acceleration = (Vector3){0};

    // Build tree bounds (simple cubic bounds)
    OctreeNode* root = octree_create((Vector3){0}, 5000.0f);

    world_foreach_body(world, b) if (b->mass > 0) octree_insert(root, b);
    world_foreach_body(world, b) if (b->mass > 0) octree_apply(root, b, G);

    octree_integrate(world, dt);

    // TODO: free octree (recursive)
}
