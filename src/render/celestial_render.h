#pragma once

#include "../world/world.h"
#include "raylib.h"
#include "renderer.h"

void celestial_render_init(void);
void celestial_render_shutdown(void);
void celestial_render_world(const World* world, const RenderContext* ctx);
