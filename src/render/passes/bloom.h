#pragma once
#include <raylib.h>
#include <stdbool.h>

typedef struct Bloom {
    int width;
    int height;

    RenderTexture2D scene;
    RenderTexture2D bright;
    RenderTexture2D blur1;
    RenderTexture2D blur2;

    Shader extract;
    Shader blur;
    Shader combine;

    int loc_threshold;
    int loc_texelSize;
    int loc_direction;
    int loc_bloomIntensity;

    bool enabled;
} Bloom;

void bloom_init(Bloom* bloom, int width, int height);
void bloom_shutdown(Bloom* bloom);
void bloom_resize(Bloom* bloom, int width, int height);

void bloom_begin_scene(Bloom* bloom);
void bloom_end_scene(Bloom* bloom);

void bloom_draw(Bloom* bloom);
