#pragma once
#include "../world/world.h"
#include "renderer.h"

void celestial_render_init(World* world);
void celestial_render_shutdown(World* world);
void celestial_render_world(const World* world, const RenderContext* ctx);
