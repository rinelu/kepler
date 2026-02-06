#pragma once
#include <raylib.h>

typedef struct Glow {
    Shader shader;

    int loc_mvp;
    int loc_model;
    int loc_viewPos;
    int loc_color;
    int loc_intensity;
} Glow;

void glow_init(Glow* g);
void glow_shutdown(Glow* g);
void glow_draw_mesh(Glow* g, Mesh mesh, Material* material, Matrix model, Matrix mvp, Color color, float intensity, Vector3 camera_pos);
