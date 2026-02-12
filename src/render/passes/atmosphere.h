#pragma once
#include <raylib.h>

typedef struct Atmosphere {
    Shader shader;
    Material material;

    int loc_mvp;
    int loc_model;
    int loc_camera_pos;
    int loc_color;
    int loc_planet_center;
    int loc_light;
    int loc_radius;
    int loc_thickness;
    int loc_intensity;
} Atmosphere;

void atmosphere_init(Atmosphere* a);
void atmosphere_shutdown(Atmosphere* a);
void atmosphere_begin(Atmosphere* a);
void atmosphere_draw(Atmosphere* a, Mesh mesh, Matrix model, Matrix mvp,
                     Vector3 camera_pos, Vector3 planet_center, Color color, float radius, float thickness, float intensity);
void atmosphere_end(void);
