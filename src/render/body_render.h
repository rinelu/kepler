#pragma once
#include "renderer.h"

enum { LOD_NEAR, LOD_MID, LOD_FAR };

static int pick_lod(float dist)
{
    if (dist < 40.0f)  return LOD_NEAR;
    if (dist < 200.0f) return LOD_MID;
    return LOD_FAR;
}

void body_render_init(BodyRender* body);
void body_render_shutdown(BodyRender* body);
void body_render(const Body* body, const RenderContext* ctx);
