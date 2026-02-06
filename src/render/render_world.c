#include "render_world.h"
#include "world/world.h"
#include "render/passes/celestial_render.h"
#include "math/utils/utils.h"

void build_lights(const World* world, RenderContext* ctx)
{
    ctx->light_count = 0;

    Body* b;
    world_foreach_body(world, b) {
        if (!b->visible) continue;
        if (!b->render.emits_light) continue;
        if (ctx->light_count >= MAX_LIGHTS) continue;;

        RenderLight* l = &ctx->lights[ctx->light_count++];

        l->position  = b->position;
        Vector3 srgb = blackbody_to_rgb(b->render.temperature);
        l->color = srgb_to_linear(srgb);
        l->intensity = b->render.light_intensity;
    }
}

void render_world(const World* world, RenderContext* ctx)
{
    build_lights(world, ctx);
    celestial_render_world(world, ctx);
}
