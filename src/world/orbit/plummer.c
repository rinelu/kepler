#include "orbit.h"
#include <raylib.h>
#include <math.h>
#include <assert.h>
#include "core/config.h"

static float frand(void)
{
    return GetRandomValue(1, 1000000) / 1000000.0f;
}

static Vector3 random_unit_vector(void)
{
    float z = 2.0f * frand() - 1.0f;
    float t = 2.0f * PI * frand();
    float r = sqrtf(1.0f - z*z);

    return (Vector3){r * cosf(t), r * sinf(t), z};
}

// Plummer radius sampling
static float sample_radius(float a)
{
    float u = frand();
    float x = powf(u, 2.0f / 3.0f);
    return a * x / sqrtf(1.0f - x);
}

// Rejection sampling for speed
static float sample_speed(float r, float a, float M, float G)
{
    float v_esc = sqrtf(2.0f * G * M / sqrtf(r*r + a*a));

    while (1) {
        float x = frand();
        float y = frand();

        float v = x * v_esc;
        float f = v*v * powf(1.0f - (v*v)/(v_esc*v_esc), 3.5f);

        if (y < f)
            return v;
    }
}

void orbit_plummer(World* world, float a)
{
    assert(world);
    int N = world_body_count(world);
    assert(N > 0);

    float G = config()->world.gravity_constant;

    float m_tot = 0.0f;
    Body* b;
    world_foreach_body(world, b) m_tot += b->mass;
    assert(m_tot > 0.0f);

    world_foreach_body(world, b) {
        float r = sample_radius(a);
        Vector3 dir = random_unit_vector();
        b->position = Vector3Scale(dir, r);

        float v = sample_speed(r, a, m_tot, G);
        Vector3 vdir = random_unit_vector();
        b->velocity = Vector3Scale(vdir, v);

        b->acceleration = (Vector3){0};
        b->damping = 0.0f;
    }

    Vector3 com = {0};
    Vector3 P   = {0};

    world_foreach_body(world, b) {
        com = Vector3Add(com, Vector3Scale(b->position, b->mass));
        P   = Vector3Add(P, Vector3Scale(b->velocity, b->mass));
    }

    com = Vector3Scale(com, 1.0f / m_tot);
    Vector3 vcom = Vector3Scale(P, 1.0f / m_tot);

    world_foreach_body(world, b) {
        b->position = Vector3Subtract(b->position, com);
        b->velocity = Vector3Subtract(b->velocity, vcom);
    }
}
