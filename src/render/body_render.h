#pragma once
#include "render/render_context.h"
#include "world/body.h"
#include <raymath.h>

enum { LOD_NEAR, LOD_MID, LOD_FAR };

static inline int pick_lod_projected(float projected_radius)
{
    if (projected_radius > 0.15f) return LOD_NEAR;
    if (projected_radius > 0.04f) return LOD_MID;
    return LOD_FAR;
}

static inline bool body_visible(const Body* b, const RenderContext* ctx, float* out_proj_radius)
{
    Vector3 d = Vector3Subtract(b->position, ctx->camera_pos);
    float dist2 = Vector3DotProduct(d, d);

    // squared projected radius
    float pr2 = (b->render.radius * b->render.radius) / dist2;
    const float min_pr2 = 0.0025f * 0.0025f;

    if (pr2 < min_pr2) return false;

    *out_proj_radius = sqrtf(pr2);
    return true;
}

void body_render_init(BodyRender* body);
void body_render_shutdown(BodyRender* body);

void body_render_begin(const BodyShader* sh, const RenderContext* ctx);
void body_render_draw(const Body* body, const RenderContext* ctx);
void body_render_end(void);
