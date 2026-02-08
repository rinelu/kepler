#include "celestial_render.h"
#include "render/body_render.h"
#include "engine/engine.h"
#include "editor/selection.h"
#include <raymath.h>
#include <rlgl.h>
#include <assert.h>
#include "core/global.h"
#include "render/passes/atmosphere.h"
#include "world/world.h"

void celestial_render_init(World* world)
{
    Body* body;
    world_foreach_body(world, body) body_render_init(&body->render);
}

void celestial_render_world(const World* world, const RenderContext* ctx)
{
    Renderer* r = engine()->renderer;
    {
        BodyShader* sh = &g_default_shader;
        body_render_begin(sh, ctx);

        Body* b;
        world_foreach_body(world, b)
            body_render_draw(b, ctx);
        body_render_end();
    }

    atmosphere_begin(&r->atmosphere);
    Body* b;
    world_foreach_body(world, b) {
        if (!b->render.use_atmosphere) continue;

        float proj_radius;
        if (!body_visible(b, ctx, &proj_radius)) continue;
        if (proj_radius < 0.01f) continue;

        float scale = 1.0f + b->render.atmosphere_thickness;

        Matrix model = MatrixMultiply(
                MatrixScale(b->render.radius * scale, b->render.radius * scale, b->render.radius * scale),
                MatrixTranslate(b->position.x, b->position.y, b->position.z));

        Matrix mvp = MatrixMultiply(ctx->proj, MatrixMultiply(ctx->view, model));

        atmosphere_draw( &r->atmosphere, b->render.mesh->sphere_model[LOD_FAR].meshes[0], &b->render.mesh->sphere_model[LOD_FAR].materials[0],
            model, mvp, ctx->camera_pos, b->render.atmosphere_color,
            b->render.radius, b->render.atmosphere_thickness * b->render.radius, b->render.atmosphere_intensity);
    }

    atmosphere_end();

}

void celestial_render_selection(const World* world)
{
    Renderer* r = engine()->renderer;

    Body* body;
    world_foreach_body(world, body) {
        if (!body_is_selected(body)) continue;

        selection_draw(&r->selection, body->position, body->render.radius * 1.23f, WHITE);
    }
}

void celestial_render_shutdown(World* world)
{
    Body* body;
    world_foreach_body(world, body) body_render_shutdown(&body->render);
}
