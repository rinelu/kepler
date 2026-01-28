#include "physics_system.h"
#include "core/config.h"
#include "core/engine.h"
#include "raymath.h"
#include "world/body.h"
#include <stdlib.h>

// NOTE: Should i not use Barness Hut for a body in less than 34?

void physics_step(GravBodyView* grav, PhysicsBodyView* phys, int count, float dt, const OctreeConfig* cfg)
{
    for (int i = 0; i < count; i++)  *phys[i].velocity     = Vector3Add(*phys[i].velocity, Vector3Scale(*grav[i].acceleration, 0.5f * dt));
    for (int i = 0; i < count; i++)  *phys[i].position     = Vector3Add(*phys[i].position, Vector3Scale(*phys[i].velocity, dt));
    for (int i = 0; i < count; i++)  *grav[i].acceleration = (Vector3){0};

    octree_compute_gravity(grav, count, cfg);

    for (int i = 0; i < count; i++) *phys[i].velocity = Vector3Add(*phys[i].velocity, Vector3Scale(*grav[i].acceleration, 0.5f * dt));
}

void physics_system_update(float dt)
{
    World* world = engine()->world;

    static GravBodyView* grav    = NULL;
    static PhysicsBodyView* phys = NULL;
    static int capacity          = 0;

    if (capacity < world->capacity) {
        capacity = world->capacity;
        grav = realloc(grav, capacity * sizeof(*grav));
        phys = realloc(phys, capacity * sizeof(*phys));
    }
    int count = 0;

    world_build_physics_views(world, grav, phys, &count);

    OctreeConfig cfg = {
        .theta     = config()->world.bh_theta,
        .softening = config()->world.softening,
        .G         = config()->world.gravity_constant
    };

    physics_step(grav, phys, count, dt, &cfg); 
}
