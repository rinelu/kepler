#pragma once
#include "engine/scheduler.h"
#include "world/world.h"
#include <stdbool.h>

bool kepler_init();
bool kepler_should_close();

void kepler_create_planets(World* world);
void kepler_init_systems(Scheduler* s);

void kepler_update();
void kepler_render();
void kepler_shutdown();
