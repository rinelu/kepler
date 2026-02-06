#include "predict_render.h"
#include "predict/predict_state.h"
#include "raymath.h"

static void draw_thick_line(Vector3 a, Vector3 b, Color c)
{
    DrawLine3D(a, b, c);
    DrawLine3D(Vector3Add(a, (Vector3){0.01f, 0, 0}), Vector3Add(b, (Vector3){0.01f, 0, 0}), c);
}

void predict_render_draw(const PredictState* ps)
{
    if (!ps || !ps->enabled) return;
    if (!ps->positions || ps->steps < 2) return;

    int stride = ps->stride > 0 ? ps->stride : 1;
    Color c = ps->frozen
        ? (Color){120, 120, 255, 120}
        : (Color){150, 150, 150, 120};

    for (int b = 0; b < ps->body_count; b++) {
        Vector3* pts = ps->positions[b];

        for (int i = 0; i + stride < ps->steps; i += stride) {
            if (((i / stride) & 1) == 0) {
                draw_thick_line(pts[i], pts[i + stride], c); // optional
            }
        }

        DrawSphere(pts[ps->steps - 1], 0.05f, c);
    }
}
