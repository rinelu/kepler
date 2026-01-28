#pragma once
#include <stdbool.h>
#include <stdint.h>

#define TIME_MAX_ACCUMULATED 0.25f

typedef struct Time {
    float real_dt;
    float accumulator;

    float fixed_dt;
    float scaled_dt;
    float time_scale;

    bool paused;
    bool single_step;
    bool simulation_enabled;
    bool use_fixed_timestep;

    uint64_t step;  // deterministic step counter

    float dt;
} Time;

void time_init(Time* time);
void time_begin_frame(Time* time);
void time_end_frame(Time* time);

bool time_should_step(Time* time);
void time_consume_step(Time* time);

void time_step_once(Time* time);
void time_set_paused(Time* time, bool paused);
void time_set_scale(Time* time, float scale);
void time_set_fixed(Time* time, bool fixed);
