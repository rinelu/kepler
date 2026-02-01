#include <imgui.h>
#include "core/engine.h"
#include "raymath.h"

extern "C" {
#include "core/config.h"
}

void draw_config_panel()
{
    if (!ImGui::Begin("Configuration")) {
        ImGui::End();
        return;
    }

    #define RETURN goto end;

    auto& world    = g_config.world;
    auto& sim      = g_config.sim;
    auto& camera   = g_config.camera;
    auto& renderer = *g_engine.renderer;
    auto& time     = g_engine.time;
    auto& predict  = g_engine.predict;

    // ---------------- World ----------------
    if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat("Gravity", &world.gravity_constant, 0.001f, 0.0f, 100.0f);
        ImGui::DragFloat("BH Theta", &world.bh_theta, 0.01f, 0.1f, 2.0f);
        ImGui::DragFloat("Softening", &world.softening, 0.0001f, 0.0f, 1.0f);
        ImGui::DragFloat("Max Velocity", &world.max_velocity, 0.1f, 0.0f, 1e6f);
        ImGui::DragInt("Max Bodies", &g_config.world.max_bodies, 1, 1, 10'000'000);
    }

    // ---------------- Simulation ----------------
    if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat("Time Scale", &time.time_scale, 0.01f, 0.0f, 100.0f);
        time.time_scale = Clamp(time.time_scale, 1.0f, TIME_MAX_FRAME_STEPS);
        ImGui::Checkbox("Paused", &time.paused);
        ImGui::Checkbox("VSync", &renderer.vsync);

        ImGui::Text("Resolution: %dx%d", sim.screen_width, sim.screen_height);
    }

    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Invert X axis", &camera.invert_mouse_x);
        ImGui::Checkbox("Invert Y axis", &camera.invert_mouse_y);
    }

    if (ImGui::CollapsingHeader("Prediction", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Enable Prediction", &predict.enabled);
        ImGui::Checkbox("Freeze Prediction", &g_engine.predict.frozen);
        if (ImGui::Button("Rebuild Prediction")) predict_state_mark_dirty(&g_engine.predict);

        if (!predict.enabled) RETURN;
        bool changed = false;
        changed |= ImGui::DragInt("Steps", &predict.steps, 10, 1, PREDICT_MAX_STEPS);
        ImGui::DragInt("Stride", &predict.stride, 1, 1, 100);

        predict.steps  = Clamp(predict.steps, 1, PREDICT_MAX_STEPS);
        predict.stride = Clamp(predict.stride, 1, predict.steps);
        predict.dt     = Clamp(predict.dt, 0.0001f, 1.0f);

        if (changed) predict_state_mark_dirty(&g_engine.predict);
    }

end:
    ImGui::End();
}
