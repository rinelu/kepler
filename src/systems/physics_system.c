#include "physics_system.h"
#include "core/config.h"
#include "core/engine.h"
#include "octree/octree.h"
#include "raymath.h"
#include "world/body.h"
#include <stdlib.h>

static float max_acceleration(GravBodyView* grav, int count)
{
    float max_a = 0.0f;
    for (int i = 0; i < count; i++) {
        float a = Vector3Length(*grav[i].acceleration);
        if (a > max_a) max_a = a;
    }
    return max_a;
}

static void direct_compute_gravity(GravBodyView* bodies, int count, float G, float softening)
{
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            Vector3 d = Vector3Subtract(*bodies[j].position,*bodies[i].position);
            float r2 = Vector3LengthSqr(d) + softening * softening;
            float inv_r3 = 1.0f / (sqrtf(r2) * r2);

            Vector3 a = Vector3Scale(d, G * bodies[j].mass * inv_r3);
            *bodies[i].acceleration = Vector3Add(*bodies[i].acceleration, a);
        }
    }
}

static int compute_substeps(float dt, float max_acc, const OctreeConfig* cfg)
{
    if (max_acc <= 1e-6f) return 1;

    float safe_dt = cfg->softening / sqrtf(max_acc);
    int steps = (int)ceilf(dt / safe_dt);

    if (steps < 1) steps = 1;
    if (steps > MAX_SUBSTEPS) steps = MAX_SUBSTEPS;

    return steps;
}

void compute_gravity(GravBodyView* grav, int count, const OctreeConfig* cfg)
{
    if (count == 0) return;
    if (count < 50) {
        direct_compute_gravity(grav, count, cfg->G, cfg->softening);
        return;
    }
    bh_compute_gravity(grav, count, cfg);
}

void physics_step(GravBodyView* grav, PhysicsBodyView* phys, int count, float dt, const OctreeConfig* cfg)
{
    for (int i = 0; i < count; i++) *grav[i].acceleration = (Vector3){0};

    compute_gravity(grav, count, cfg);
    float max_a = max_acceleration(grav, count);
    int substeps = compute_substeps(dt, max_a, cfg);
    float h = dt / substeps;

    for (int s = 0; s < substeps; s++) {
        for (int i = 0; i < count; i++) *phys[i].velocity = Vector3Add(*phys[i].velocity,Vector3Scale(*grav[i].acceleration, 0.5f * h));
        for (int i = 0; i < count; i++) *phys[i].position = Vector3Add(*phys[i].position, Vector3Scale(*phys[i].velocity, h));
        for (int i = 0; i < count; i++) *grav[i].acceleration = (Vector3){0};
        compute_gravity(grav, count, cfg);
        for (int i = 0; i < count; i++) *phys[i].velocity = Vector3Add(*phys[i].velocity, Vector3Scale(*grav[i].acceleration, 0.5f * h));
    }
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
