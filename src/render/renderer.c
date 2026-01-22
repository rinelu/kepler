#include "renderer.h"

#include <raylib.h>
#include <stdlib.h>
#include <assert.h>

#include "render_world.h"

Renderer* renderer_create(int width, int height, const char* title, bool vsync)
{
    Renderer* renderer = (Renderer*)calloc(1, sizeof(Renderer));
    assert(renderer);

    renderer->width  = width;
    renderer->height = height;
    renderer->vsync  = vsync;
    renderer->clear_color = (Color){ 10, 10, 14, 255 };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    if (vsync) SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(width, height, title);
    assert(IsWindowReady());

    SetTargetFPS(0);

    return renderer;
}

void renderer_destroy(Renderer* renderer)
{
    if (!renderer) return;

    CloseWindow();
    free(renderer);
}

void renderer_begin_frame(Renderer* renderer, Camera camera)
{
    assert(renderer);

    BeginDrawing();
    ClearBackground(renderer->clear_color);
    BeginMode3D(camera);
}

void renderer_end_frame(Renderer* renderer)
{
    assert(renderer);

    EndMode3D();
    EndDrawing();
}

void renderer_draw_world(Renderer* renderer, const World* world, Camera camera)
{
    render_world(world, camera);
}


void renderer_resize(Renderer* renderer, int width, int height)
{
    assert(renderer);

    renderer->width  = width;
    renderer->height = height;

    SetWindowSize(width, height);
}

void renderer_reload_assets(Renderer* renderer)
{
    assert(renderer);
}
