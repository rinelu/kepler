#pragma once

#include <stdbool.h>
#include <raylib.h>
#include "render/passes/atmosphere.h"
#include "world/world.h"
#include "camera/camera.h"
#include "predict/predict_state.h"
#include "render/passes/bloom.h"
#include "render/passes/glow.h"
#include "editor/selection.h"
#include "render/render_context.h"

typedef struct {
    int   width;
    int   height;
    bool  vsync;
    Color clear_color;

    RenderContext ctx;
    Bloom bloom;
    Glow glow;
    Selection selection;
    Atmosphere atmosphere;
} Renderer;

Renderer* renderer_create(int width, int height, const char* title, bool vsync);
RenderContext renderer_build_context(CameraState* camera);
void renderer_update_context(RenderContext* ctx, CameraState* camera_state);
void renderer_destroy(Renderer* renderer);

void renderer_begin_frame(Renderer* renderer);
void renderer_render_world(Renderer* renderer, const World* world, PredictState* predict);
void renderer_render_gui(Renderer* renderer);
void renderer_end_frame(Renderer* renderer);

void renderer_resize(Renderer* renderer, int width, int height);
void renderer_reload_assets(Renderer* renderer);
