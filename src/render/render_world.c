#include "render_world.h"
#include "world/world.h"
#include "render/passes/celestial_render.h"
#include "math/utils/utils.h"

void build_lights(const World* world, RenderContext* ctx)
{
    static uint64_t last_revision = 0;
    if (world->revision == last_revision) return;

    last_revision = world->revision;
    ctx->light_count = 0;

    Body* b;
    world_foreach_body(world, b) {
        if (!b->render.emits_light) continue;
        if (ctx->light_count == 32) break;

        RenderLight* l = &ctx->lights[ctx->light_count++];
        l->position = b->position;
        l->color = srgb_to_linear(blackbody_to_rgb(b->render.temperature));
        l->intensity = b->render.light_intensity;
    }
}

void render_world(const World* world, RenderContext* ctx)
{
    build_lights(world, ctx);
    celestial_render_world(world, ctx);
}
