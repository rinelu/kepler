#include "core/config.h"
#include "orbit.h"
#include <stddef.h>

void orbit_keplerian(World* world) {
    float G = config()->world.gravity_constant;
    int N = world_body_count(world);
    if (N < 2) return;

    Body* central = NULL;
    float max_mass = -1.0f;

    Body* b;
    world_foreach_body(world, b) {
        if (b->mass > max_mass) {
            max_mass = b->mass;
            central = b;
        }
    }

    if (!central) return;

    Vector3 axis = (Vector3){0, 1, 0};
    world_foreach_body(world, b) {
        if (b == central) {
            b->velocity = (Vector3){0};
            continue;
        }

        Vector3 r_vec = Vector3Subtract(b->position, central->position);
        float r = Vector3Length(r_vec);
        if (r < 1e-3f) {
            b->velocity = (Vector3){0};
            continue;
        }

        float v_mag = sqrtf(G * central->mass / r);
        Vector3 v_dir = Vector3CrossProduct(axis, r_vec);
        v_dir = Vector3Normalize(v_dir);
        b->velocity = Vector3Scale(v_dir, v_mag);
    }

    Vector3 com_pos, com_vel;
    float total_mass;
    compute_com(world, &com_pos, &com_vel, &total_mass);
    world_foreach_body(world, b) {
        b->velocity = Vector3Subtract(b->velocity, com_vel);
    }
}

