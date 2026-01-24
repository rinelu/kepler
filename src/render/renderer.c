#include "renderer.h"

#include <raylib.h>
#include <stdlib.h>
#include <assert.h>
#include "gui/imgui_layer.h"
#include "rlgl.h"

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
    // SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(width, height, title);
    assert(IsWindowReady());

    ImGuiLayer_Init();

    SetTargetFPS(0);

    return renderer;
}

void renderer_update_context(RenderContext* ctx, CameraState* camera_state)
{
    Camera camera = camera_state->camera;
    ctx->camera     = camera;
    ctx->camera_pos = camera.position;
    ctx->view       = GetCameraMatrix(camera);

    float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
    ctx->proj = MatrixPerspective(camera.fovy * DEG2RAD, aspect, 0.01f, 100000.0f);
    ctx->time = GetTime();
};

RenderContext renderer_build_context(CameraState* camera)
{
    RenderContext ctx = {0};
    renderer_update_context(&ctx, camera);
    return ctx;
}

void renderer_destroy(Renderer* renderer)
{
    if (!renderer) return;

    ImGuiLayer_Shutdown();
    CloseWindow();
    free(renderer);
}

void renderer_begin_frame(Renderer* renderer)
{
    assert(renderer);

    BeginDrawing();
    ClearBackground(renderer->clear_color);
}

void renderer_render_world(Renderer* renderer, const World* world)
{
    BeginMode3D(renderer->ctx.camera);
    rlEnableDepthTest();
    rlClearScreenBuffers();
    rlEnableBackfaceCulling();

    render_world(world, &renderer->ctx);

    rlDisableBackfaceCulling();
    rlDisableDepthTest();
    EndMode3D();
}

void renderer_render_gui(Renderer* renderer)
{
    (void)renderer;

    rlDrawRenderBatchActive();
    rlDisableDepthTest();
    rlDisableBackfaceCulling();
    rlSetTexture(0);

    ImGuiLayer_Begin();
    ImGuiLayer_Draw();
    ImGuiLayer_End();
}

void renderer_end_frame(Renderer* renderer)
{
    assert(renderer);

    EndDrawing();
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
