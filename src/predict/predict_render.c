#include "predict_render.h"
#include "rlgl.h"

void predict_render_draw(const PredictState* ps)
{
    if (!ps || !ps->enabled || !ps->positions) return;
    if (ps->stored_steps < 2) return;

    Color c = ps->frozen
        ? (Color){120, 120, 255, 120}
        : (Color){150, 150, 150, 120};

    rlPushMatrix();
    rlEnableDepthTest();
    rlEnableColorBlend();
    rlDisableTexture();
    rlSetBlendMode(RL_BLEND_ALPHA);

    rlBegin(RL_LINES);
    rlColor4ub(c.r, c.g, c.b, c.a);

    for (int b = 0; b < ps->body_count; b++) {
        const Vector3* base = &ps->positions[b * ps->stored_steps];

        for (int i = 0; i < ps->stored_steps - 1; i++) {
            rlVertex3f(base[i].x,     base[i].y,     base[i].z);
            rlVertex3f(base[i + 1].x, base[i + 1].y, base[i + 1].z);
        }
    }

    rlEnd();
    rlPopMatrix();
}
