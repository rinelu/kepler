#include "orbit.h"
#include <math.h>
#include <assert.h>

void orbit_virial_rotating(World* world)
{
    assert(world);
    int N = world_body_count(world);
    if (N < 2) return;

    Vector3 com_pos, com_vel;
    float total_mass;
    compute_com(world, &com_pos, &com_vel, &total_mass);

    Body* b;
    world_foreach_body(world, b) {
        b->position = Vector3Subtract(b->position, com_pos);
        b->velocity = (Vector3){0};
    }

    Vector3 axis = Vector3Normalize((Vector3){0.2f, 1.0f, 0.1f});
    // Vector3 axis = deterministic_axis(world);

    float K = 0.0f;
    world_foreach_body(world, b) {
        Vector3 r = b->position;
        if (Vector3LengthSqr(r) < 1e-6f) continue;

        Vector3 t = Vector3CrossProduct(axis, r);
        float t_len = Vector3Length(t);
        if (t_len < 1e-6f) continue;

        t = Vector3Normalize(t);
        b->velocity = Vector3Scale(t, 1.0f);
        K += 0.5f * b->mass * Vector3LengthSqr(b->velocity);
    }

    float U = compute_total_potential(world);
    if (K > 0.0f) {
        float scale = sqrtf(-0.5f * U / K);
        world_foreach_body(world, b) {
            b->velocity = Vector3Scale(b->velocity, scale);
            b->acceleration = (Vector3){0};
            b->damping = 0.0f;
        }
    }

    compute_com(world, &com_pos, &com_vel, &total_mass);
    world_foreach_body(world, b) {
        b->velocity = Vector3Subtract(b->velocity, com_vel);
    }
}
