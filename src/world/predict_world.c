#include "predict_world.h"
#include "core/config.h"
#include "systems/physics_system.h"
#include <stdlib.h>

void predict_world_clone(const World* src, PredictWorld* dst)
{
    dst->count  = world_body_count(src);
    dst->bodies = malloc(sizeof(PredictBody) * dst->count);

    int i = 0;
    Body* b;
    world_foreach_body(src, b) {
        dst->bodies[i++] = (PredictBody){
            .position     = b->position,
            .velocity     = b->velocity,
            .acceleration = b->acceleration,
            .mass         = b->mass,
            .damping      = b->damping,
        };
    }
}

void predict_build_physics_views(PredictWorld* pw, GravBodyView* grav, PhysicsBodyView* phys)
{
    for (int i = 0; i < pw->count; i++) {
        grav[i] = (GravBodyView){
            .position     = &pw->bodies[i].position,
            .acceleration = &pw->bodies[i].acceleration,
            .mass         = pw->bodies[i].mass
        };

        phys[i] = (PhysicsBodyView){
            .position     = &pw->bodies[i].position,
            .velocity     = &pw->bodies[i].velocity,
            .acceleration = &pw->bodies[i].acceleration,
            .mass         = &pw->bodies[i].mass,
            .damping      = &pw->bodies[i].damping
        };

    }
}

void predict_run(const World* world,int steps, float dt, Vector3* out_positions)
{
    PredictWorld pw;
    predict_world_clone(world, &pw);

    GravBodyView grav[pw.count];
    PhysicsBodyView phys[pw.count];

    OctreeConfig cfg = {
        .theta     = config()->world.bh_theta,
        .softening = config()->world.softening,
        .G         = config()->world.gravity_constant
    };

    for (int i = 0; i < steps; i++) {
        predict_build_physics_views(&pw, grav, phys);
        physics_step(grav, phys, pw.count, dt, &cfg);
        out_positions[i] = pw.bodies[0].position;
    }

    predict_world_free(&pw);
}

void predict_world_free(PredictWorld* pw)
{
    free(pw->bodies);
    pw->bodies = NULL;
    pw->count  = 0;
}
