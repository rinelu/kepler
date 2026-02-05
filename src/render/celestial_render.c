#include "celestial_render.h"
#include "body_render.h"
#include "core/engine.h"
#include "render/selection.h"
#include <raymath.h>
#include <rlgl.h>
#include <assert.h>

void celestial_render_init(World* world)
{
    Body* body;
    world_foreach_body(world, body) body_render_init(&body->render);
}

void celestial_render_world(const World* world, const RenderContext* ctx)
{
    Renderer* r = engine()->renderer;

    Body* body;
    world_foreach_body(world, body) body_render(body, ctx);
}

void celestial_render_selection(const World* world)
{
    Renderer* r = engine()->renderer;

    Body* body;
    world_foreach_body(world, body) {
        if (!body_is_selected(body)) continue;

        selection_draw(
            &r->selection,
            body->position,
            body->render.radius * 1.23f,
            WHITE);
    }
}

void celestial_render_shutdown(World* world)
{
    Body* body;
    world_foreach_body(world, body) body_render_shutdown(&body->render);
}
