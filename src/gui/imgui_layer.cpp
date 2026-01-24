#include "imgui_layer.h"

#include "imgui_impl_raylib.h"
#include "rlImGui.h"

// Forward declare
void draw_config_panel();

void ImGuiLayer_Init()
{
    rlImGuiSetup(true);
}

void ImGuiLayer_Begin()
{
    rlImGuiBegin();
}

void ImGuiLayer_Draw()
{
    draw_config_panel();
}

void ImGuiLayer_End()
{
    rlImGuiEnd();
}

void ImGuiLayer_Shutdown()
{
    ImGui_ImplRaylib_Shutdown();
}
