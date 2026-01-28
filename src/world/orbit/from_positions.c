#include "orbit.h"
#include "core/config.h"

static float compute_total_potential(World* world)
{
    float G = config()->world.gravity_constant;
    float U = 0.0f;

    Body* bi;
    Body* bj;

    world_foreach_body(world, bi) {
        world_foreach_body(world, bj) {
            if (bi >= bj) continue;

            Vector3 d = Vector3Subtract(bi->position, bj->position);
            float r = Vector3Length(d) + 1e-6f;
            U -= G * bi->mass * bj->mass / r;
        }
    }
    return U;
}

// NOTE: This is sometime broken
void orbit_from_positions(World* world)
{
    assert(world);
    int N = world_body_count(world);
    if (N < 2) return;

    float U = compute_total_potential(world);
    float K_target = -0.5f * U;   // virial theorem

    float K = 0.0f;
    Body* b;

    world_foreach_body(world, b) {
        Vector3 dir = {
            GetRandomValue(-1000,1000) / 1000.0f,
            GetRandomValue(-1000,1000) / 1000.0f,
            GetRandomValue(-1000,1000) / 1000.0f
        };
        dir = Vector3Normalize(dir);

        b->velocity = dir;
        K += 0.5f * b->mass;
    }

    float scale = sqrtf(K_target / K);

    world_foreach_body(world, b) {
        b->velocity = Vector3Scale(b->velocity, scale);
    }

    Vector3 P = {0};
    float M = 0.0f;

    world_foreach_body(world, b) {
        P = Vector3Add(P, Vector3Scale(b->velocity, b->mass));
        M += b->mass;
    }

    Vector3 vcom = Vector3Scale(P, 1.0f / M);

    world_foreach_body(world, b) {
        b->velocity = Vector3Subtract(b->velocity, vcom);
        b->acceleration = (Vector3){0};
        b->damping = 0.0f;
    }
}
