#include "body_render.h"
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
    Body* body;
    world_foreach_body(world, body) body_render(body, ctx);
}

void celestial_render_shutdown(World* world)
{
    Body* body;
    world_foreach_body(world, body) body_render_shutdown(&body->render);
}
