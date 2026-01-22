#include "celestial_render.h"
#include <assert.h>
#include <raylib.h>

static Mesh  sphere_mesh;
static Model sphere_model;
static Shader default_planet_shader;

void celestial_render_init(void)
{
    sphere_mesh  = GenMeshSphere(1.0f, 64, 64);
    sphere_model = LoadModelFromMesh(sphere_mesh);

    default_planet_shader = LoadShader(
        "assets/shaders/planet.vs",
        "assets/shaders/planet.fs"
    );
}

void celestial_render_shutdown(void)
{
    UnloadShader(default_planet_shader);
    UnloadModel(sphere_model);
}

static void cache_uniforms(BodyRender* r)
{
    if (r->loc_mvp != -1) return;

    r->loc_mvp       = GetShaderLocation(r->shader, "mvp");
    r->loc_time      = GetShaderLocation(r->shader, "time");
    r->loc_radius    = GetShaderLocation(r->shader, "radius");
    r->loc_color     = GetShaderLocation(r->shader, "baseColor");
    r->loc_light_dir = GetShaderLocation(r->shader, "lightDir");
}

static void render_body(const Body* body, Camera camera)
{
    if (!body->visible) return;

    BodyRender* r = (BodyRender*)&body->render;
    Shader shader = (r->shader.id != 0) ? r->shader : default_planet_shader;

    r->shader = shader;
    cache_uniforms(r);

    float time = GetTime();
    Vector3 light_dir = (Vector3){ -0.3f, -1.0f, -0.2f };

    SetShaderValue(shader, r->loc_time, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, r->loc_radius, &body->radius, SHADER_UNIFORM_FLOAT);

    float color[4] = {
        r->base_color.r / 255.0f,
        r->base_color.g / 255.0f,
        r->base_color.b / 255.0f,
        r->base_color.a / 255.0f
    };

    SetShaderValue(shader, r->loc_color, color, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, r->loc_light_dir, &light_dir, SHADER_UNIFORM_VEC3);

    sphere_model.materials[0].shader = shader;

    DrawModelEx(
        sphere_model,
        body->position,
        (Vector3){ 0, 1, 0 },
        body->rotation.angle * RAD2DEG,
        (Vector3){ body->render.radius, body->render.radius, body->render.radius },
        WHITE
    );
}

void celestial_render_world(const World* world, Camera camera)
{
    Body* body;
    world_foreach_body(world, body) {
        render_body(body, camera);
    }
}
