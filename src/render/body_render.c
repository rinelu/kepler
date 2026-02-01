#include "body_render.h"
#include "raylib.h"
#include "renderer.h"
#include <raymath.h>
#include <rlgl.h>
#include <assert.h>

// TODO: write shader manager.
static BodyShader default_shader;
static BodyMesh   default_mesh;
static bool ba_loaded;

void body_render_init(BodyRender* render)
{
    if (!ba_loaded) {
        default_mesh.sphere_mesh[LOD_NEAR] = GenMeshSphere(1.0f, 64, 64);
        default_mesh.sphere_mesh[LOD_MID]  = GenMeshSphere(1.0f, 32, 32);
        default_mesh.sphere_mesh[LOD_FAR]  = GenMeshSphere(1.0f, 12, 12);

        for (int i = 0; i < 3; i++)
            default_mesh.sphere_model[i] = LoadModelFromMesh(default_mesh.sphere_mesh[i]);

        default_shader.shader = LoadShader("assets/shaders/planet.vert", "assets/shaders/planet.frag");

        default_shader.loc_mvp      = GetShaderLocation(default_shader.shader, "mvp");
        default_shader.loc_model    = GetShaderLocation(default_shader.shader, "model");
        default_shader.loc_color    = GetShaderLocation(default_shader.shader, "baseColor");
        default_shader.loc_light    = GetShaderLocation(default_shader.shader, "lightDir");
        default_shader.loc_emissive = GetShaderLocation(default_shader.shader, "emissiveIntensity");
        default_shader.loc_light_count     = GetShaderLocation(default_shader.shader, "lightCount");
        default_shader.loc_light_pos       = GetShaderLocation(default_shader.shader, "lightPos");
        default_shader.loc_light_color     = GetShaderLocation(default_shader.shader, "lightColor");
        default_shader.loc_light_intensity = GetShaderLocation(default_shader.shader, "lightIntensity");
        ba_loaded = true;
    }

    render->shader = &default_shader;
    render->mesh   = &default_mesh;
}

void body_render_shutdown(BodyRender* render)
{
    if (render->shader != &default_shader) {
        UnloadShader(default_shader.shader);
    }

    if (render->mesh != &default_mesh) {
        for (int i = 0; i < 3; i++)
            UnloadModel(default_mesh.sphere_model[i]);
    }
}

void body_render(const Body* body, const RenderContext* ctx)
{
    if (!body->visible) return;
    const BodyRender* render = &body->render;

    float dist = Vector3Distance(ctx->camera_pos, body->position);
    int lod = pick_lod(dist);

    const Model* model = &render->mesh->sphere_model[lod];
    model->materials[0].shader = render->shader->shader;

    Matrix model_mtx = MatrixMultiply(
        MatrixScale(render->radius, body->render.radius, body->render.radius),
        MatrixTranslate(body->position.x, body->position.y, body->position.z));

    Matrix mvp = MatrixMultiply(ctx->proj, MatrixMultiply(ctx->view, model_mtx));

    float color[4] = {
        render->base_color.r / 255.0f,
        render->base_color.g / 255.0f,
        render->base_color.b / 255.0f,
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

    BodyShader* bh = render->shader;
    BeginShaderMode(bh->shader);
        SetShaderValueMatrix(bh->shader, bh->loc_mvp, mvp);
        SetShaderValueMatrix(bh->shader, bh->loc_model, model_mtx);
        SetShaderValue(bh->shader, bh->loc_emissive, &body->render.emissive_strength, SHADER_UNIFORM_FLOAT);
        SetShaderValue(bh->shader, bh->loc_color, color, SHADER_UNIFORM_VEC4);
        SetShaderValue(bh->shader, bh->loc_light_count, &count, SHADER_UNIFORM_INT);
        SetShaderValueV(bh->shader, bh->loc_light_pos, pos, SHADER_UNIFORM_VEC3, count);
        SetShaderValueV(bh->shader, bh->loc_light_color, col, SHADER_UNIFORM_VEC3, count);
        SetShaderValueV(bh->shader, bh->loc_light_intensity, inten, SHADER_UNIFORM_FLOAT, count);

        DrawMesh(model->meshes[0], model->materials[0], model_mtx);
    EndShaderMode();

    if (body->render.use_atmosphere) {
        // TODO: Implement atmosphere
    }
}
