#include "relax_system.h"
#include "core/config.h"
#include "engine/engine.h"
#include "world/world.h"
#include "core/log.h"
#include <raylib.h>

// TODO: This relaxation system is bad. Will fix this later.

static bool  g_active      = false;
static float g_damping     = 0.05f;    // strong initial damping
static float g_decay       = 0.9995f;  // exponential decay per step
static float g_min_damping = 0.0001f;
static int g_warmup_steps  = 120;      // ~2 seconds at 60 Hz

static Vector3 pick_global_axis(void)
{
    Vector3 a = {
        GetRandomValue(-1000,1000) / 1000.0f,
        GetRandomValue(-1000,1000) / 1000.0f,
        GetRandomValue(-1000,1000) / 1000.0f
    };
    return Vector3Normalize(a);
}

static void seed_angular_momentum(World* world)
{
    Vector3 axis = pick_global_axis();

    Body* b;
    world_foreach_body(world, b) {
        Vector3 r = b->position;
        float dist = Vector3Length(r);
        if (dist < 1e-3f) continue;

        Vector3 r_hat = Vector3Normalize(r);
        Vector3 tangent = Vector3CrossProduct(axis, r_hat);

        if (Vector3LengthSqr(tangent) < 1e-6f) {
            tangent = Vector3CrossProduct(r_hat, (Vector3){0, 1, 0});
            if (Vector3LengthSqr(tangent) < 1e-6f)
                tangent = Vector3CrossProduct(r_hat, (Vector3){1, 0, 0});
        }

        tangent = Vector3Normalize(tangent);
        float v = 0.3f * sqrtf(config()->world.gravity_constant / dist);
        b->velocity = Vector3Add(b->velocity, Vector3Scale(tangent, v));
    }
}

void relaxation_init(void)
{
    g_active        = true;
    g_damping       = 0.05f;
    g_warmup_steps  = 120;

    World* world = engine()->world;
    Body* b;
    world_foreach_body(world, b) {
        b->velocity     = (Vector3){0};
        b->acceleration = (Vector3){0};
        b->damping      = 0.0f;
    }

    seed_angular_momentum(world);

    LOG_INFO("[RELAX] initialized with angular momentum seed");
}

bool relaxation_active(void)
{
    return g_active;
}

void relaxation_update(float dt)
{
    (void)dt;
    if (!g_active) return;

    World* world = engine()->world;
    Body* b;

    if (g_warmup_steps > 0) {
        g_warmup_steps--;
        return;
    }

    g_damping *= g_decay;
    world_foreach_body(world, b) {
        b->velocity = Vector3Scale(b->velocity, 1.0f - g_damping);
    }

    if (g_damping < g_min_damping) {
        g_damping = 0.0f;
        g_active = false;

        LOG_INFO("[RELAX] completed");
        return;
    }
}
