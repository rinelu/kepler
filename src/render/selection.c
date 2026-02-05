#include "selection.h"
#include "core/engine.h"
#include <rlgl.h>
#include <raymath.h>

void selection_init(Selection* s)
{
    s->shader = LoadShader("assets/shaders/selection.vert", "assets/shaders/selection.frag");

    s->loc_mvp   = GetShaderLocation(s->shader, "mvp");
    s->loc_model = GetShaderLocation(s->shader, "model");
    s->loc_time  = GetShaderLocation(s->shader, "time");
    s->loc_color = GetShaderLocation(s->shader, "outlineColor");
}

void selection_shutdown(Selection* s)
{
    UnloadShader(s->shader);
}

void selection_draw(Selection* s, Vector3 pos, float radius, Color color)
{
    if (!s) return;

    float time = (float)GetTime();
    Vector4 nColor = ColorNormalize(color);

    Camera3D cam = engine()->camera.camera;

    // Build billboard basis
    Vector3 forward = Vector3Normalize(Vector3Subtract(cam.target, cam.position));
    Vector3 right   = Vector3Normalize(Vector3CrossProduct(forward, cam.up));
    Vector3 up      = Vector3CrossProduct(right, forward);

    right = Vector3Scale(right, radius);
    up    = Vector3Scale(up, radius);

    rlDisableDepthMask();
    rlEnableDepthTest();   // still depth-test against planets

    BeginShaderMode(s->shader);

    SetShaderValue(s->shader, s->loc_time, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(s->shader, s->loc_color, &nColor, SHADER_UNIFORM_VEC4);

    rlBegin(RL_QUADS);
        rlColor4f(1, 1, 1, 1);
        rlTexCoord2f(0, 0);
        rlVertex3f(pos.x - right.x - up.x, pos.y - right.y - up.y, pos.z - right.z - up.z);
        rlTexCoord2f(1, 0);
        rlVertex3f(pos.x + right.x - up.x, pos.y + right.y - up.y, pos.z + right.z - up.z);
        rlTexCoord2f(1, 1);
        rlVertex3f(pos.x + right.x + up.x, pos.y + right.y + up.y, pos.z + right.z + up.z);
        rlTexCoord2f(0, 1);
        rlVertex3f(pos.x - right.x + up.x, pos.y - right.y + up.y, pos.z - right.z + up.z);
    rlEnd();

    EndShaderMode();

    rlEnableDepthMask();
    rlDisableDepthTest();
}

