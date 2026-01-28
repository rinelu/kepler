#pragma once
#include "core/scheduler.h"
#include "world/world.h"
#include <stdbool.h>

bool app_init();
bool app_should_close();

void app_create_planets(World* world);
void app_init_systems(Scheduler* s);

void app_update();
void app_handle_input();
void app_render();
void app_shutdown();
