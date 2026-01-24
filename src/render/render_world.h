#pragma once
#include "../world/world.h"
#include "renderer.h"

void render_world(const World* world, RenderContext* ctx);
void render_world_reload(void);
