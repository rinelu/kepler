#pragma once
#include "core/config.h"
#include "world/world.h"

void orbit_plummer(World* world, float a);
void orbit_virial_rotating(World* world);
void orbit_keplerian(World* world);

static void compute_com(World* world, Vector3* pos, Vector3* vel, float* mass)
{
    *pos = (Vector3){0};
    *vel = (Vector3){0};
    *mass = 0.0f;

    Body* b;
    world_foreach_body(world, b) {
        *pos = Vector3Add(*pos, Vector3Scale(b->position, b->mass));
        *vel = Vector3Add(*vel, Vector3Scale(b->velocity, b->mass));
        *mass += b->mass;
    }

    *pos = Vector3Scale(*pos, 1.0f / *mass);
    *vel = Vector3Scale(*vel, 1.0f / *mass);
}

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

static Vector3 deterministic_axis(World* world)
{
    Vector3 axis = {0,0,0};
    Body* b;
    world_foreach_body(world, b) {
        axis.x += b->position.x * b->position.x * b->mass;
        axis.y += b->position.y * b->position.y * b->mass;
        axis.z += b->position.z * b->position.z * b->mass;
    }

    // pick the largest component
    if (axis.x >= axis.y && axis.x >= axis.z) axis = (Vector3){1,0,0};
    else if (axis.y >= axis.x && axis.y >= axis.z) axis = (Vector3){0,1,0};
    else axis = (Vector3){0,0,1};

    return Vector3Normalize(axis);
}


