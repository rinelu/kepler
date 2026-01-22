#pragma once

#include "../world/world.h"
#include "raylib.h"

void celestial_render_init(void);
void celestial_render_shutdown(void);
void celestial_render_world(const World *world, Camera camera);
