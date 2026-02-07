#include <imgui.h>
#include "camera/camera.h"
#include "engine/engine.h"
#include "raymath.h"
#include "world/world.h"

extern "C" {
#include "core/config.h"
}

void draw_camera_header()
{
    if (!ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        return;
    auto& ecamera = g_engine.camera;
    auto& camera  = g_config.camera;
    static const char* camera_mode_labels[] = { "Free", "Orbit", "Follow" };

    ImGui::Checkbox("Invert X axis", &camera.invert_mouse_x);
    ImGui::Checkbox("Invert Y axis", &camera.invert_mouse_y);

    ImGui::SliderFloat("Mouse Sensitivity", &ecamera.sensitivity, 0.0005f, 0.01f, "%.4f");
    ImGui::SliderFloat("Move Speed", &ecamera.move_speed, 0.1f, 500.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("Zoom Speed", &ecamera.zoom_speed, 0.01f, 5.0f);

    int mode = (int)ecamera.mode;
    if (ImGui::Combo("Mode", &mode, camera_mode_labels, IM_ARRAYSIZE(camera_mode_labels)))
        ecamera.mode = (Kepler_CameraMode)mode;

    if (ImGui::Button("Reset Camera"))
        camera_init(&ecamera, ecamera.world);

    ImGui::Separator();

    switch (ecamera.mode) {
    case CAMERA_MODE_FREE: {
        ImGui::TextUnformatted("FREE Camera");

        ImGui::DragFloat3("Position", &ecamera.camera.position.x, 0.1f);
        ImGui::DragFloat("Yaw",   &ecamera.yaw,   0.01f);
        ImGui::DragFloat("Pitch", &ecamera.pitch, 0.01f, -PI * 0.49f, PI * 0.49f);

        ImGui::Text("Velocity");
        ImGui::DragFloat3("##Velocity", &ecamera.velocity.x, 0.01f);

        if (ImGui::Button("Zero Velocity"))
            ecamera.velocity = { 0, 0, 0 };
    } break;

    case CAMERA_MODE_ORBIT: {
        ImGui::TextUnformatted("ORBIT Camera");

        ImGui::DragFloat3("Target", &ecamera.target_goal.x, 0.1f);
        ImGui::SliderFloat("Distance", &ecamera.distance_goal, 0.01f, 1e9f, "%.3f", ImGuiSliderFlags_Logarithmic);

        ImGui::DragFloat("Yaw", &ecamera.yaw_goal, 0.01f);
        ImGui::DragFloat("Pitch", &ecamera.pitch_goal, 0.01f, -PI * 0.49f, PI * 0.49f);

        if (ImGui::Button("Snap to Target")) {
            ecamera.target   = ecamera.target_goal;
            ecamera.yaw      = ecamera.yaw_goal;
            ecamera.pitch    = ecamera.pitch_goal;
            ecamera.distance = ecamera.distance_goal;
        }

        if (ImGui::TreeNode("Debug State")) {
            ImGui::Text("Current Target: (%.3f, %.3f, %.3f)", ecamera.target.x, ecamera.target.y, ecamera.target.z);
            ImGui::Text("Current Distance: %.3f", ecamera.distance);
            ImGui::TreePop();
        }
    } break;

    case CAMERA_MODE_FOLLOW: {
        ImGui::TextUnformatted("FOLLOW Camera");
        ImGui::Separator();

        if (engine()->world->count <= 0) {
            ImGui::TextDisabled("No bodies available");
            break;
        }

        const char* preview = "<none>";
        Body* b = nullptr;

        world_foreach_body(engine()->world, b) {
            if (ecamera.follow_id.value == b->id.value) {
                preview = b->name;
                break;
            }
        }

        if (ImGui::BeginCombo("Follow Target", preview)) {
            world_foreach_body(engine()->world, b) {
                bool selected = (ecamera.follow_id.value == b->id.value);
                if (ImGui::Selectable(b->name, selected))
                    camera_set_follow(&ecamera, b->id);
                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        Body* target = world_get_body(engine()->world, ecamera.follow_id);
        if (target) {
            ImGui::Text("Target Position: (%.3f, %.3f, %.3f)", target->position.x, target->position.y, target->position.z);
        }
    } break;

    default: break;
    }
}

void draw_prediction_header()
{
    auto& predict = g_engine.predict;

    if (!ImGui::CollapsingHeader("Prediction", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::Checkbox("Enable Prediction", &predict.enabled);
    ImGui::Checkbox("Freeze Prediction", &predict.frozen);

    if (ImGui::Button("Rebuild Prediction")) predict_state_mark_dirty(&predict);

    if (!predict.enabled) return;

    bool changed = false;
    changed |= ImGui::DragInt("Steps", &predict.steps, 10, 1, PREDICT_MAX_STEPS);
    changed |= ImGui::DragInt("Stride", &predict.stride, 1, 0, 100);
    changed |= ImGui::DragFloat("dt", &predict.dt, 0.0005f, 0.0001f, 1.0f, "%.4f");

    predict.steps  = Clamp(predict.steps, 1, PREDICT_MAX_STEPS);
    predict.stride = Clamp(predict.stride, 0, predict.steps);
    predict.dt     = Clamp(predict.dt, 0.0001f, 1.0f);

    if (changed) predict_state_mark_dirty(&predict);
}

void draw_config_panel()
{
    if (!ImGui::Begin("Configuration")) {
        ImGui::End();
        return;
    }

    auto& world    = g_config.world;
    auto& sim      = g_config.sim;
    auto& renderer = *g_engine.renderer;
    auto& time     = g_engine.time;

    // ---------------- World ----------------
    if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool changed = false;
        changed |= ImGui::DragFloat("Gravity", &world.gravity_constant, 0.001f, 0.0f, 100.0f);
        changed |= ImGui::DragFloat("BH Theta", &world.bh_theta, 0.01f, 0.1f, 2.0f);
        changed |= ImGui::DragFloat("Softening", &world.softening, 0.0001f, 0.0f, 1.0f);
        changed |= ImGui::DragFloat("Max Velocity", &world.max_velocity, 0.1f, 0.0f, 1e6f);
        changed |= ImGui::DragInt("Max Bodies", &g_config.world.max_bodies, 1, 1, 10'000'000);

        if (changed) predict_state_mark_dirty(&g_engine.predict);
    }

    // ---------------- Simulation ----------------
    if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat("Time Scale", &time.time_scale, 0.01f, 0.0f, 100.0f);
        time.time_scale = Clamp(time.time_scale, 1.0f, TIME_MAX_FRAME_STEPS);
        ImGui::Checkbox("Paused", &time.paused);
        ImGui::Checkbox("VSync", &renderer.vsync);

        ImGui::Text("Resolution: %dx%d", sim.screen_width, sim.screen_height);
    }

    draw_camera_header();

    // ---------------- Prediction ----------------
    draw_prediction_header();

    ImGui::End();
}
