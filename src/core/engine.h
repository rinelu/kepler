#pragma once
#include <assert.h>

#include "world/world.h"
#include "render/renderer.h"
#include "core/scheduler.h"
#include "predict/predict_state.h"
#include "core/time.h"

typedef struct Engine {
    Time time;
    Scheduler* scheduler;

    World*     world;
    Renderer*  renderer;
    PredictState predict;
    CameraState camera;
} Engine;

extern Engine g_engine;

static inline Engine* engine(void)
{
    assert(&g_engine);
    return &g_engine;
}
