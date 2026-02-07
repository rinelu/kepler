#include "predict_state.h"
#include "core/config.h"
#include "systems/physics_system.h"
#include "world/predict_world.h"
#include <stdlib.h>
#include <string.h>

static void free_buffers(PredictState* ps)
{
    free(ps->positions);
    free(ps->velocities);
    ps->positions  = NULL;
    ps->velocities = NULL;
}

void predict_state_init(PredictState* ps)
{
    *ps = (PredictState){
        .enabled       = true,
        .frozen        = false,
        .needs_rebuild = true,
        .steps         = 400,
        .stride        = 2,
        .dt            = 0.02f,
    };
}

void predict_state_shutdown(PredictState* ps)
{
    free_buffers(ps);
    *ps = (PredictState){0};
}

void predict_state_mark_dirty(PredictState* ps)
{
    ps->needs_rebuild = true;
}

void predict_state_update(PredictState* ps, const World* world)
{
    if (!ps->enabled) return;
    if (ps->frozen && !ps->needs_rebuild) return;

    int count = world_body_count(world);
    if (count == 0) return;

    ps->effective_stride = (ps->stride <= 0) ? 1 : ps->stride;

    ps->stored_steps = (ps->steps + ps->effective_stride - 1) / ps->effective_stride;
    if (ps->stored_steps < 2) ps->stored_steps = 2;

    bool realloc = count != ps->body_count || ps->stored_steps != ps->capacity_steps || !ps->positions;

    if (realloc) {
        free(ps->positions);
        free(ps->velocities);

        ps->positions  = malloc(sizeof(Vector3) * count * ps->stored_steps);
        ps->velocities = malloc(sizeof(Vector3) * count * ps->stored_steps);

        ps->body_count     = count;
        ps->capacity_steps = ps->stored_steps;
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
        ps->positions[b * ps->stored_steps] = *phys[b].position;
    }

    int store_step = 1;
    for (int step = 1; step < ps->steps; step++) {
        physics_step(grav, phys, count, ps->dt, &cfg);

        if ((step % ps->effective_stride) == 0 && store_step < ps->stored_steps) {
            for (int b = 0; b < count; b++) {
                ps->positions[b * ps->stored_steps + store_step] = *phys[b].position;
            }
            store_step++;
        }
    }

    predict_world_free(&pw);
    ps->needs_rebuild = false;
}

