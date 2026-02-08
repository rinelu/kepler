#include "atmosphere.h"
#include <assert.h>
#include <rlgl.h>

void atmosphere_init(Atmosphere* a)
{
    *a = (Atmosphere){0};

    a->shader = LoadShader("assets/shaders/atmosphere.vert", "assets/shaders/atmosphere.frag");

    a->loc_mvp        = GetShaderLocation(a->shader, "mvp");
    a->loc_model      = GetShaderLocation(a->shader, "model");
    a->loc_camera_pos = GetShaderLocation(a->shader, "cameraPos");
    a->loc_color      = GetShaderLocation(a->shader, "atmoColor");
    a->loc_radius     = GetShaderLocation(a->shader, "planetRadius");
    a->loc_thickness  = GetShaderLocation(a->shader, "atmoThickness");
    a->loc_intensity  = GetShaderLocation(a->shader, "intensity");
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
}

void atmosphere_draw(Atmosphere* a, Mesh mesh, Material* material, Matrix model, Matrix mvp, Vector3 camera_pos,
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

    DrawMesh(mesh, *material, model);
}
