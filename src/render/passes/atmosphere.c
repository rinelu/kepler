#include "atmosphere.h"
#include "engine/engine.h"
#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

void atmosphere_init(Atmosphere* a)
{
    *a = (Atmosphere){0};

    a->shader = LoadShader("assets/shaders/atmosphere.vert", "assets/shaders/atmosphere.frag");
    a->material = LoadMaterialDefault();
    a->material.shader = a->shader;

    a->loc_mvp        = GetShaderLocation(a->shader, "mvp");
    a->loc_model      = GetShaderLocation(a->shader, "model");
    a->loc_camera_pos = GetShaderLocation(a->shader, "cameraPos");
    a->loc_color      = GetShaderLocation(a->shader, "atmoColor");
    a->loc_radius     = GetShaderLocation(a->shader, "planetRadius");
    a->loc_thickness  = GetShaderLocation(a->shader, "atmoThickness");
    a->loc_intensity  = GetShaderLocation(a->shader, "intensity");
    a->loc_planet_center = GetShaderLocation(a->shader, "planetCenter");
    a->loc_light         = GetShaderLocation(a->shader, "lightDir");
}

void atmosphere_shutdown(Atmosphere* a)
{
    if (!a) return;
    UnloadShader(a->shader);
}

void atmosphere_begin(Atmosphere* a)
{
    rlEnableDepthTest();
    rlDisableDepthMask();
    rlDisableBackfaceCulling();
    rlEnableColorBlend();
    rlSetBlendMode(RL_BLEND_ADDITIVE);
    BeginShaderMode(a->shader);
}

void atmosphere_end(void)
{
    EndShaderMode();
    rlSetBlendMode(RL_BLEND_ALPHA);
    rlDisableColorBlend();
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
    rlDisableDepthTest();
}

void atmosphere_draw(Atmosphere* a, Mesh mesh, Matrix model, Matrix mvp, Vector3 camera_pos, Vector3 planet_center,
                     Color color, float radius, float thickness, float intensity)
{
    float col[3] = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f
    };

    SetShaderValueMatrix(a->shader, a->loc_mvp, mvp);
    SetShaderValueMatrix(a->shader, a->loc_model, model);
    SetShaderValue(a->shader, a->loc_camera_pos, &camera_pos, SHADER_UNIFORM_VEC3);
    SetShaderValue(a->shader, a->loc_color, col, SHADER_UNIFORM_VEC3);
    SetShaderValue(a->shader, a->loc_radius, &radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(a->shader, a->loc_thickness, &thickness, SHADER_UNIFORM_FLOAT);
    SetShaderValue(a->shader, a->loc_intensity, &intensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(a->shader, a->loc_planet_center, &planet_center, SHADER_UNIFORM_VEC3);

    Vector3 light_dir = Vector3Normalize(Vector3Subtract(engine()->renderer->ctx.lights[0].position, planet_center));
    SetShaderValue(a->shader, a->loc_light, &light_dir, SHADER_UNIFORM_VEC3);

    DrawMesh(mesh, a->material, model);
}
