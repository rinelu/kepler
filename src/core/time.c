#include "time.h"
#include "core/config.h"
#include "raymath.h"
#include <string.h>

float GetFrameTime();

void time_init(Time* t)
{
    memset(t, 0, sizeof(*t));

    t->time_scale         = config()->sim.time_scale;
    t->fixed_dt           = 1.0f / 60.0f;
    t->simulation_enabled = true;
    t->use_fixed_timestep = true;
}

void time_begin_frame(Time* t)
{
    t->real_dt = GetFrameTime();

    if (!t->simulation_enabled) {
        t->dt = 0.0f;
        return;
    }

    float scaled = Clamp(t->real_dt * t->time_scale, 0.0f, t->fixed_dt * 4.0f);


    if (t->use_fixed_timestep) {
        if (!t->paused || t->single_step) {
            t->accumulator += scaled;
            if (t->accumulator > TIME_MAX_ACCUMULATED)
                t->accumulator = TIME_MAX_ACCUMULATED;
        }
        t->dt = t->fixed_dt;
    }else {
        t->dt = scaled;
    }
}

bool time_should_step(Time* t)
{
    if (!t->simulation_enabled) return false;

    if (t->use_fixed_timestep) {
        return (t->accumulator >= t->fixed_dt) || t->single_step;
    }

    return t->dt > 0.0f;
}

void time_consume_step(Time* t)
{
    if (t->use_fixed_timestep) {
        t->accumulator -= t->fixed_dt;
    }

    t->step++;

    if (t->single_step) {
        t->single_step = false;
        t->paused = true;
    }
}

void time_end_frame(Time* t)
{
    // nothing yet
}

void time_set_paused(Time* t, bool paused)
{
    t->paused = paused;
}

void time_step_once(Time* t)
{
    t->single_step = true;
    t->paused = false;
}

void time_set_fixed(Time* t, bool fixed)
{
    t->use_fixed_timestep = fixed;
}
