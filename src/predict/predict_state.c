#include "predict_state.h"
#include "core/config.h"
#include "systems/physics_system.h"
#include "world/predict_world.h"
#include <stdlib.h>

static Vector3** alloc_tracks(int bodies, int steps) {
    Vector3** t = calloc(bodies, sizeof(Vector3*));
    for (int i = 0; i < bodies; i++)
        t[i] = calloc(steps, sizeof(Vector3));
    return t;
}

static void free_tracks(Vector3** t, int bodies) {
    if (!t) return;
    for (int i = 0; i < bodies; i++) free(t[i]);
    free(t);
}

void predict_state_init(PredictState* ps)
{
    *ps = (PredictState){
        .enabled       = true,
        .frozen        = true,
        .needs_rebuild = true,
        .allocated_steps = 0,
        .steps         = 1000,
        .stride        = 2,
        .dt            = 0.02f,
    };
}

void predict_state_shutdown(PredictState* ps)
{
    free_tracks(ps->positions, ps->body_count);
    free_tracks(ps->velocities, ps->body_count);
    *ps = (PredictState){0};
}

void predict_state_update(PredictState* ps, const World* world)
{
    if (!ps->enabled && ps->frozen && !ps->needs_rebuild) return;

    int count = world_body_count(world);
    if (count == 0) return;

    if (count != ps->body_count || ps->steps != ps->allocated_steps) {
        free_tracks(ps->positions, ps->body_count);
        free_tracks(ps->velocities, ps->body_count);

        ps->positions       = alloc_tracks(count, ps->steps);
        ps->velocities      = alloc_tracks(count, ps->steps);
        ps->body_count      = count;
        ps->allocated_steps = ps->steps;
        ps->needs_rebuild   = true;
    }

    PredictWorld pw;
    predict_world_clone(world, &pw);

    GravBodyView grav[count];
    PhysicsBodyView phys[count];
    predict_build_physics_views(&pw, grav, phys);

    OctreeConfig cfg = {
        .theta     = config()->world.bh_theta,
        .softening = config()->world.softening,
        .G         = config()->world.gravity_constant
    };

    for (int b = 0; b < count; b++) {
        ps->positions[b][0]  = *phys[b].position;
        ps->velocities[b][0] = *phys[b].velocity;
    }

    for (int step = 1; step < ps->steps; step++) {
        physics_step(grav, phys, count, ps->dt, &cfg);
        for (int b = 0; b < count; b++) {
            ps->positions[b][step]  = *phys[b].position;
            ps->velocities[b][step] = *phys[b].velocity;
        }
    }

    predict_world_free(&pw);
    ps->needs_rebuild = false;
}

void predict_state_mark_dirty(PredictState* ps)
{
    ps->needs_rebuild = true;
}
