#include <imgui.h>
extern "C" {
#include "core/config.h"
}

void draw_config_panel()
{
    if (!ImGui::Begin("Configuration")) {
        ImGui::End();
        return;
    }

    // ---------------- World ----------------
    if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat("Gravity", &g_config.world.gravity_constant, 0.001f, 0.0f, 100.0f);
        ImGui::DragFloat("BH Theta", &g_config.world.bh_theta, 0.01f, 0.1f, 2.0f);
        ImGui::DragFloat("Softening", &g_config.world.softening, 0.0001f, 0.0f, 1.0f);
        ImGui::DragFloat("Max Velocity", &g_config.world.max_velocity, 0.1f, 0.0f, 1e6f);
        ImGui::DragInt("Max Bodies", &g_config.world.max_bodies, 1, 1, 10'000'000);
    }

    // ---------------- Physics ----------------
    if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Enabled", &g_config.physics.enabled);
        ImGui::DragFloat("Damping", &g_config.physics.damping_default, 0.001f, 0.0f, 1.0f);
    }

    // ---------------- Simulation ----------------
    if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat("Time Scale", &g_config.sim.time_scale, 0.01f, 0.0f, 100.0f);
        ImGui::Checkbox("Paused", &g_config.sim.paused);
        ImGui::Checkbox("VSync", &g_config.sim.vsync);

        ImGui::Text("Resolution: %dx%d", g_config.sim.screen_width, g_config.sim.screen_height);
    }

    ImGui::End();
}
