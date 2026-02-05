#include "gui_layer.h"

#include "core/engine.h"
#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include "raylib-nuklear.h"

void NuklearLayer_Init(void)
{
    engine()->nuklear = InitNuklear(10); // font size or atlas scaling
}

bool NuklearLayer_CaptureInput(void)
{
    struct nk_context* ctx = engine()->nuklear;
    if (!ctx) return false;

    return ctx->input.mouse.grab ||
           ctx->input.mouse.buttons[NK_BUTTON_LEFT].down ||
           ctx->input.keyboard.keys[NK_KEY_TEXT_INSERT_MODE].down;
}

void NuklearLayer_Begin(void)
{
    struct nk_context* ctx = engine()->nuklear;
    if (!ctx) return;

    UpdateNuklear(ctx);
}

void NuklearLayer_Draw(void)
{
    struct nk_context* ctx = engine()->nuklear;
    if (!ctx) return;
}

void NuklearLayer_End(void)
{
    struct nk_context* ctx = engine()->nuklear;
    if (!ctx) return;

    DrawNuklear(ctx);
}

void NuklearLayer_Shutdown(void)
{
    struct nk_context* ctx = engine()->nuklear;
    if (!ctx) return;

    UnloadNuklear(ctx);
    engine()->nuklear = NULL;
}
