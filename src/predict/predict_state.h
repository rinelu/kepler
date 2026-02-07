#pragma once
#include "../world/world.h"
#include <stdbool.h>
#include <raylib.h>

typedef struct {
    bool enabled;
    bool frozen;
    bool needs_rebuild;

    int steps;
    int stride;
    int effective_stride;

    int stored_steps;
    int capacity_steps;

    float dt;
    int body_count;

    Vector3* positions;
    Vector3* velocities;
} PredictState;

#define PRED_POS(ps, b, s) ((ps)->positions[(b)*(ps)->stored_steps + (s)])

#ifdef __cplusplus
extern "C" {
#endif

void predict_state_init(PredictState* ps);
void predict_state_shutdown(PredictState* ps);
void predict_state_update(PredictState* ps, const World* world);
void predict_state_mark_dirty(PredictState* ps);

#ifdef __cplusplus
}
#endif
