#include "input.h"
#include <raylib.h>
#include "core/engine.h"
#include "core/time.h"
#include "editor/pick_body.h"
#include "gui/imgui_layer.h"
#include "world/world_ids.h"

void handle_body_picking(WorldID picked)
{
    if (!world_id_is_valid(picked) || world_id_equal(engine()->selected_body, picked)) {
        engine()->selected_body = WORLD_ID_INVALID;
        return;
    }

    engine()->selected_body = picked;
}

void input_update(void)
{
    Engine* e = engine();

    if (!ImGuiLayer_CaptureInput() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        handle_body_picking(pick_body(e->world));
    }

    if (IsKeyPressed(KEY_SPACE)) {
        time_set_paused(&e->time, !e->time.paused);
    }

    if (IsKeyPressed(KEY_N)) {
        time_step_once(&e->time);
    }

    if (IsKeyPressed(KEY_F)) {
        time_set_fixed(&e->time, !e->time.use_fixed_timestep);
    }
}
