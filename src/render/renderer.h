#pragma once

#include <stdbool.h>
#include <raylib.h>
#include "../world/world.h"

typedef struct{
    int   width;
    int   height;
    bool  vsync;
    Color clear_color;
} Renderer;

Renderer* renderer_create(int width, int height, const char* title, bool vsync);
void      renderer_destroy(Renderer* renderer);

void renderer_begin_frame(Renderer* renderer, Camera camera);
void renderer_end_frame(Renderer* renderer);

void renderer_draw_world(Renderer* renderer, const World* world, Camera camera);

void renderer_resize(Renderer* renderer, int width, int height);
void renderer_reload_assets(Renderer* renderer);
