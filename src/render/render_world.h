#pragma once
#include "../world/world.h"
#include "render/renderer.h"

void render_world(const World* world, RenderContext* ctx);
void render_world_reload(void);
