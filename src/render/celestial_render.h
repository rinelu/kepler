#pragma once
#include "../world/world.h"
#include "core/engine.h"
#include "renderer.h"

void celestial_render_init(World* world);
void celestial_render_shutdown(World* world);
void celestial_render_world(const World* world, const RenderContext* ctx);
void celestial_render_selection(const World* world);

static bool body_is_selected(const Body* body)
{
    WorldID sel = engine()->selected_body;
    return world_id_is_valid(sel) && world_id_equal(sel, body->id);
}
