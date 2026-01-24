#include "render_world.h"

#include <assert.h>
#include "../world/world.h"
#include "celestial_render.h"

#define MAX_LIGHTS 4

typedef struct {
    Vector3 position;
    Vector3 color;
    float intensity;
} GPULight;

static int gather_lights(const World* world, GPULight* out_lights, int max_lights)
{
    int count = 0;

    Body* b;
    world_foreach_body(world, b) {
        if (!b->visible) continue;
        if (!b->render.emits_light) continue;
        if (count >= max_lights) break;

        out_lights[count++] = (GPULight){
            .position  = b->position,
            .color     = (Vector3){
                b->render.base_color.r / 255.0f,
                b->render.base_color.g / 255.0f,
                b->render.base_color.b / 255.0f
            },
            .intensity = b->render.light_intensity
        };
    }

    return count;
}

void render_world(const World* world, RenderContext* ctx)
{
    ctx->light_dir = Vector3Normalize((Vector3){ -1.0f, -0.4f, -0.2f });

    celestial_render_world(world, ctx);
}

void render_world_reload(void)
{
}
