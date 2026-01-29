#include "render_world.h"
#include "world/world.h"
#include "render/celestial_render.h"

static Vector3 blackbody_to_rgb(float kelvin)
{
    float t = kelvin / 100.0f;
    float r, g, b;

    // Red
    if (t <= 66.0f) r = 1.0f;
    else {
        r = t - 60.0f;
        r = 329.698727446f * powf(r, -0.1332047592f) / 255.0f;
    }

    // Green
    if (t <= 66.0f) {
        g = 99.4708025861f * logf(t) - 161.1195681661f;
    } else {
        g = t - 60.0f;
        g = 288.1221695283f * powf(g, -0.0755148492f);
    }
    g /= 255.0f;

    // Blue
    if (t >= 66.0f) b = 1.0f;
    else if (t <= 19.0f) b = 0.0f;
    else {
        b = t - 10.0f;
        b = 138.5177312231f * logf(b) - 305.0447927307f;
        b /= 255.0f;
    }

    return (Vector3){
        Clamp(r, 0.0f, 1.0f),
        Clamp(g, 0.0f, 1.0f),
        Clamp(b, 0.0f, 1.0f)
    };
}

void build_lights(const World* world, RenderContext* ctx)
{
    ctx->light_count = 0;

    Body* b;
    world_foreach_body(world, b) {
        if (!b->visible) continue;
        if (!b->render.emits_light) continue;
        if (ctx->light_count >= MAX_LIGHTS) break;

        RenderLight* l = &ctx->lights[ctx->light_count++];

        l->position  = b->position;
        l->color     = blackbody_to_rgb(b->render.temperature),
        l->intensity = b->render.light_intensity;
    }
}

void render_world(const World* world, RenderContext* ctx)
{
    build_lights(world, ctx);
    celestial_render_world(world, ctx);
}
