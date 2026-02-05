#include "gui/gui_layer.h"

#include "core/engine.h"
#include "imgui.h"
#include "rlImGui.h"
#include "world/world.h"
#include "world/world_ids.h"

// Forward declare
void draw_config_panel();
void selection_overlay_draw(World* world);
void body_params_panel(Body* b);

void ImGuiLayer_Init()
{
    rlImGuiSetup(true);
}

bool ImGuiLayer_CaptureInput(void)
{
    return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiLayer_Begin()
{
    rlImGuiBegin();
}

void ImGuiLayer_Draw()
{
    draw_config_panel();
    selection_overlay_draw(g_engine.world);
    if (world_id_is_valid(g_engine.selected_body))
        body_params_panel(world_get_body(g_engine.world, g_engine.selected_body));
}

void ImGuiLayer_End()
{
    rlImGuiEnd();
}

void ImGuiLayer_Shutdown()
{
    rlImGuiShutdown();
}
