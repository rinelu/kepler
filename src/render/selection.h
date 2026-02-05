#pragma once
#include <raylib.h>

typedef struct Selection {
    Shader shader;
    int loc_mvp;
    int loc_model;
    int loc_time;
    int loc_color;
} Selection;

void selection_init(Selection* s);
void selection_shutdown(Selection* s);
void selection_draw(Selection* s, Vector3 pos, float radius, Color color);
