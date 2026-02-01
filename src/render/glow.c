#include "glow.h"
#include <assert.h>
#include <rlgl.h>
#include <raymath.h>

void glow_init(Glow* g)
{
    assert(g);
    *g = (Glow){0};

    g->shader = LoadShader("assets/shaders/glow.vert", "assets/shaders/glow.frag");

    g->loc_mvp       = GetShaderLocation(g->shader, "mvp");
    g->loc_model     = GetShaderLocation(g->shader, "model");
    g->loc_viewPos   = GetShaderLocation(g->shader, "viewPos");
    g->loc_color     = GetShaderLocation(g->shader, "glowColor");
    g->loc_intensity = GetShaderLocation(g->shader, "intensity");
}

void glow_shutdown(Glow* g)
{
    if (!g) return;
    UnloadShader(g->shader);
}

void glow_draw_mesh(Glow* g, Mesh mesh, Material* material, Matrix model, Matrix mvp, Color color, float intensity, Vector3 camera_pos)
{
    assert(g);

    rlEnableDepthTest();
    rlDisableDepthMask();
    rlEnableBackfaceCulling();

    rlEnableColorBlend();
    rlSetBlendMode(RL_BLEND_ADDITIVE);

    float glowColor[3] = {
        powf(color.r / 255.0f, 2.2f),
        powf(color.g / 255.0f, 2.2f),
        powf(color.b / 255.0f, 2.2f)
    };

    SetShaderValueMatrix(g->shader, g->loc_mvp, mvp);
    SetShaderValueMatrix(g->shader, g->loc_model, model);
    SetShaderValue(g->shader, g->loc_viewPos, &camera_pos, SHADER_UNIFORM_VEC3);
    SetShaderValue(g->shader, g->loc_color, glowColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(g->shader, g->loc_intensity, &intensity, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(g->shader);
        DrawMesh(mesh, *material, model);
    EndShaderMode();

    rlSetBlendMode(RL_BLEND_ALPHA);
    rlDisableColorBlend();
    rlEnableDepthMask();
}
