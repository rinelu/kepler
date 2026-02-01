#include "bloom.h"
#include <rlgl.h>
#include <assert.h>

static RenderTexture2D make_rt(int w, int h)
{
    RenderTexture2D rt = LoadRenderTexture(w, h);
    SetTextureFilter(rt.texture, TEXTURE_FILTER_BILINEAR);
    return rt;
}

void bloom_init(Bloom* b, int w, int h)
{
    assert(b);
    *b = (Bloom){0};

    b->width  = w;
    b->height = h;
    b->enabled = true;

    b->scene  = make_rt(w, h);
    b->bright = make_rt(w, h);
    b->blur1  = make_rt(w, h);
    b->blur2  = make_rt(w, h);

    b->extract = LoadShader(0, "assets/shaders/bloom_extract.frag");
    b->blur    = LoadShader(0, "assets/shaders/bloom_blur.frag");
    b->combine = LoadShader(0, "assets/shaders/bloom_combine.frag");

    b->loc_threshold      = GetShaderLocation(b->extract, "threshold");
    b->loc_texelSize      = GetShaderLocation(b->blur, "texelSize");
    b->loc_direction      = GetShaderLocation(b->blur, "direction");
    b->loc_bloomIntensity = GetShaderLocation(b->combine, "bloomIntensity");

    float threshold = 0.6f;
    SetShaderValue(b->extract, b->loc_threshold, &threshold, SHADER_UNIFORM_FLOAT);

    float intensity = 1.2f;
    SetShaderValue(b->combine, b->loc_bloomIntensity, &intensity, SHADER_UNIFORM_FLOAT);
}

void bloom_shutdown(Bloom* b)
{
    UnloadRenderTexture(b->scene);
    UnloadRenderTexture(b->bright);
    UnloadRenderTexture(b->blur1);
    UnloadRenderTexture(b->blur2);

    UnloadShader(b->extract);
    UnloadShader(b->blur);
    UnloadShader(b->combine);
}

void bloom_resize(Bloom* b, int w, int h)
{
    bloom_shutdown(b);
    bloom_init(b, w, h);
}

void bloom_begin_scene(Bloom* b)
{
    if (!b->enabled) return;

    BeginTextureMode(b->scene);
    ClearBackground(BLACK);
}

void bloom_end_scene(Bloom* b)
{
    if (!b->enabled) return;
    EndTextureMode();
}

static void draw_fullscreen(Texture2D tex)
{
    DrawTexturePro(
        tex,
        (Rectangle){0, 0, tex.width, -tex.height},
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

void bloom_draw(Bloom* b)
{
    if (!b->enabled) {
        draw_fullscreen(b->scene.texture);
        return;
    }

    Vector2 texel = {
        1.0f / (float)b->width,
        1.0f / (float)b->height
    };

    // Bright pass
    BeginTextureMode(b->bright);
    ClearBackground(BLACK);
    BeginShaderMode(b->extract);
    draw_fullscreen(b->scene.texture);
    EndShaderMode();
    EndTextureMode();

    // Blur X
    Vector2 dir = {1, 0};
    SetShaderValue(b->blur, b->loc_texelSize, &texel, SHADER_UNIFORM_VEC2);
    SetShaderValue(b->blur, b->loc_direction, &dir, SHADER_UNIFORM_VEC2);

    BeginTextureMode(b->blur1);
    ClearBackground(BLACK);
    BeginShaderMode(b->blur);
    draw_fullscreen(b->bright.texture);
    EndShaderMode();
    EndTextureMode();

    // Blur Y
    dir = (Vector2){0, 1};
    SetShaderValue(b->blur, b->loc_direction, &dir, SHADER_UNIFORM_VEC2);

    BeginTextureMode(b->blur2);
    ClearBackground(BLACK);
    BeginShaderMode(b->blur);
    draw_fullscreen(b->blur1.texture);
    EndShaderMode();
    EndTextureMode();

    // Combine
    BeginShaderMode(b->combine);
    SetShaderValueTexture(b->combine, GetShaderLocation(b->combine, "sceneTex"), b->scene.texture);
    SetShaderValueTexture(b->combine, GetShaderLocation(b->combine, "bloomTex"), b->blur2.texture);

    draw_fullscreen(b->scene.texture);
    EndShaderMode();
}
