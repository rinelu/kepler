#include "body_render.h"
#include "engine/engine.h"
#include "raylib.h"
#include <raymath.h>
#include <rlgl.h>
#include <assert.h>
#include "core/global.h"
#include "render/passes/atmosphere.h"

void body_render_init(BodyRender* render)
{
    if (!g_ba_loaded) {
        g_default_mesh.sphere_mesh[LOD_NEAR] = GenMeshSphere(1.0f, 64, 64);
        g_default_mesh.sphere_mesh[LOD_MID]  = GenMeshSphere(1.0f, 32, 32);
        g_default_mesh.sphere_mesh[LOD_FAR]  = GenMeshSphere(1.0f, 12, 12);

        for (int i = 0; i < 3; i++)
            g_default_mesh.sphere_model[i] = LoadModelFromMesh(g_default_mesh.sphere_mesh[i]);

        g_default_shader.shader = LoadShader("assets/shaders/planet.vert", "assets/shaders/planet.frag");

        g_default_shader.loc_mvp      = GetShaderLocation(g_default_shader.shader, "mvp");
        g_default_shader.loc_model    = GetShaderLocation(g_default_shader.shader, "model");
        g_default_shader.loc_color    = GetShaderLocation(g_default_shader.shader, "baseColor");
        g_default_shader.loc_light    = GetShaderLocation(g_default_shader.shader, "lightDir");
        g_default_shader.loc_emissive = GetShaderLocation(g_default_shader.shader, "emissiveIntensity");
        g_default_shader.loc_light_count     = GetShaderLocation(g_default_shader.shader, "lightCount");
        g_default_shader.loc_light_pos       = GetShaderLocation(g_default_shader.shader, "lightPos");
        g_default_shader.loc_light_color     = GetShaderLocation(g_default_shader.shader, "lightColor");
        g_default_shader.loc_light_intensity = GetShaderLocation(g_default_shader.shader, "lightIntensity");
        g_ba_loaded = true;
    }
    for (int lod = 0; lod < 3; lod++) {
        g_default_mesh.sphere_model[lod].materials[0].shader = g_default_shader.shader;
    }

    render->shader = &g_default_shader;
    render->mesh   = &g_default_mesh;
}

void body_render_shutdown(BodyRender* render)
{
    if (render->shader != &g_default_shader) {
        UnloadShader(g_default_shader.shader);
    }

    if (render->mesh != &g_default_mesh) {
        for (int i = 0; i < 3; i++)
            UnloadModel(g_default_mesh.sphere_model[i]);
    }
}

void body_render_begin(const BodyShader* sh, const RenderContext* ctx)
{
    BeginShaderMode(sh->shader);
    Vector3 lightDir = Vector3Normalize(Vector3Subtract( ctx->lights[0].position, ctx->camera_pos));
    SetShaderValue(sh->shader, sh->loc_light, &lightDir, SHADER_UNIFORM_VEC3);
}


void body_render_draw(const Body* body, const RenderContext* ctx)
{
    if (!body->visible) return;

    float proj_radius;
    if (!body_visible(body, ctx, &proj_radius)) return;

    int lod = pick_lod_projected(proj_radius);
    const Model* model = &body->render.mesh->sphere_model[lod];
    BodyShader* sh = body->render.shader;

    Matrix model_mtx = MatrixMultiply(
            MatrixScale(body->render.radius, body->render.radius, body->render.radius),
            MatrixTranslate(body->position.x, body->position.y, body->position.z));

    Matrix mvp = MatrixMultiply(ctx->proj, MatrixMultiply(ctx->view, model_mtx));

    float color[4] = {
        body->render.base_color.r / 255.0f,
        body->render.base_color.g / 255.0f,
        body->render.base_color.b / 255.0f,
        1.0f
    };
    Vector3 pos[MAX_LIGHTS];
    Vector3 col[MAX_LIGHTS];
    float   inten[MAX_LIGHTS];

    int count = ctx->light_count;
    if (count > 16) count = 16;

    for (int i = 0; i < count; i++) {
        pos[i]   = ctx->lights[i].position;
        col[i]   = ctx->lights[i].color;
        inten[i] = ctx->lights[i].intensity;
    }

    SetShaderValueMatrix(sh->shader, sh->loc_mvp, mvp);
    SetShaderValueMatrix(sh->shader, sh->loc_model, model_mtx);
    SetShaderValue(sh->shader, sh->loc_color, color, SHADER_UNIFORM_VEC4);
    SetShaderValue(sh->shader, sh->loc_emissive, &body->render.emissive_strength, SHADER_UNIFORM_FLOAT);
    SetShaderValue(sh->shader, sh->loc_light_count, &count, SHADER_UNIFORM_INT);
    SetShaderValueV(sh->shader, sh->loc_light_pos, pos, SHADER_UNIFORM_VEC3, count);
    SetShaderValueV(sh->shader, sh->loc_light_color, col, SHADER_UNIFORM_VEC3, count);
    SetShaderValueV(sh->shader, sh->loc_light_intensity, inten, SHADER_UNIFORM_FLOAT, count);

    DrawMesh(model->meshes[0], model->materials[0], model_mtx);
}

void body_render_end(void)
{
    EndShaderMode();
}
