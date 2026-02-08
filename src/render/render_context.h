#pragma once
#include <raylib.h>
#define MAX_LIGHTS 16

typedef struct {
    Vector3 position;
    Vector3 color;
    float intensity;
} RenderLight;

typedef struct RenderContext {
    Camera camera;
    Matrix view;
    Matrix proj;
    Vector3 camera_pos;
    float time;

    int light_count;
    RenderLight lights[MAX_LIGHTS];
} RenderContext;
