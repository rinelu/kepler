#include "celestial_render.h"
#include "renderer.h"
#include <raymath.h>
#include <rlgl.h>
#include <assert.h>

enum { LOD_NEAR, LOD_MID, LOD_FAR };

static int loc_mvp;
static int loc_color;
static int loc_light;
static int loc_model;

static Mesh  sphere_mesh[3];
static Model sphere_model[3];
static Shader planet_shader;

static int pick_lod(float dist)
{
    if (dist < 40.0f)  return LOD_NEAR;
    if (dist < 200.0f) return LOD_MID;
    return LOD_FAR;
}

void celestial_render_init(void)
{
    sphere_mesh[LOD_NEAR] = GenMeshSphere(1.0f, 64, 64);
    sphere_mesh[LOD_MID]  = GenMeshSphere(1.0f, 32, 32);
    sphere_mesh[LOD_FAR]  = GenMeshSphere(1.0f, 12, 12);

    for (int i = 0; i < 3; i++)
        sphere_model[i] = LoadModelFromMesh(sphere_mesh[i]);

    planet_shader = LoadShader("assets/shaders/planet.vs", "assets/shaders/planet.fs");
    loc_mvp   = GetShaderLocation(planet_shader, "mvp");
    loc_color = GetShaderLocation(planet_shader, "baseColor");
    loc_light = GetShaderLocation(planet_shader, "lightDir");
    loc_model = GetShaderLocation(planet_shader, "model");
}

void celestial_render_shutdown(void)
{
    UnloadShader(planet_shader);
    for (int i = 0; i < 3; i++)
        UnloadModel(sphere_model[i]);
}

static void render_body(const Body* body, const RenderContext* ctx)
{
    if (!body->visible) return;

    float dist = Vector3Distance(ctx->camera_pos, body->position);
    int lod = pick_lod(dist);

    Model* model = &sphere_model[lod];
    model->materials[0].shader = planet_shader;

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

    BeginShaderMode(planet_shader);
    SetShaderValueMatrix(planet_shader, loc_mvp, mvp);
    SetShaderValue(planet_shader, loc_color, color, SHADER_UNIFORM_VEC4);
    SetShaderValue(planet_shader, loc_light, &ctx->light_dir, SHADER_UNIFORM_VEC3);
    SetShaderValueMatrix(planet_shader, loc_model, model_mtx);

    DrawMesh(model->meshes[0], model->materials[0], model_mtx);
    EndShaderMode();

    if (body->render.use_atmosphere) {
        rlDisableDepthMask();
        BeginBlendMode(BLEND_ADDITIVE);

        Matrix atmo = MatrixMultiply(
            MatrixScale(body->render.radius * 1.05f, body->render.radius * 1.05f, body->render.radius * 1.05f),
            MatrixTranslate(body->position.x, body->position.y, body->position.z));

        DrawMesh(model->meshes[0], model->materials[0], atmo);

        EndBlendMode();
        rlEnableDepthMask();
    }
}

void celestial_render_world(const World* world, const RenderContext* ctx)
{
    Body* body;
    world_foreach_body(world, body) render_body(body, ctx);
}
