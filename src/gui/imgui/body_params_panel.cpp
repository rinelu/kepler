#include "engine/engine.h"
#include "raylib.h"
#include "world/body.h"
#include "imgui.h"
#include "math/utils/utils.h"

void body_params_panel(Body* b)
{
    if (!b) return;
    if (!ImGui::Begin("Inspector")) {
        ImGui::End();
        return;
    }

    ImGui::PushID(b->id.value);

    ImGui::Text("%s", b->name ? b->name : "<unnamed>");
    ImGui::Separator();

    ImGui::Checkbox("Visible", &b->visible);

    bool physics_changed = false;
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        physics_changed |= ImGui::DragFloat3("Position", (float*)&b->position, 0.01f);
        physics_changed |= ImGui::DragFloat3("Velocity", (float*)&b->velocity, 0.01f);
        physics_changed |= ImGui::DragFloat3("Acceleration", (float*)&b->acceleration, 0.01f);
    }

    if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
        physics_changed |= ImGui::DragFloat("Mass", &b->mass, 0.01f, 0.0f, FLT_MAX, "%.3e");
        physics_changed |= ImGui::DragFloat("Radius", &b->radius, 0.01f, 0.0f, FLT_MAX);
        physics_changed |= ImGui::DragFloat("Damping", &b->damping, 0.001f, 0.0f, 1.0f);
    }
    if (physics_changed) predict_state_mark_dirty(&engine()->predict);

    if (ImGui::CollapsingHeader("Rotation")) {
        ImGui::DragFloat("Angle", &b->rotation.angle, 0.01f);
    }

    if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
        BodyRender* r = &b->render;

        ImGui::Checkbox("Use Surface", &r->use_surface);
        ImGui::Checkbox("Use Atmosphere", &r->use_atmosphere);
        ImGui::Checkbox("Use Lighting", &r->use_lighting);
        ImGui::Checkbox("Emits Light", &r->emits_light);

        ImGui::Separator();

        ImGui::ColorEdit4("Base Color", (float*)&r->base_color);

        ImGui::DragFloat("Roughness", &r->roughness, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Metallic", &r->metallic, 0.01f, 0.0f, 1.0f);
    }

    if (b->render.use_atmosphere && ImGui::CollapsingHeader("Atmosphere", ImGuiTreeNodeFlags_DefaultOpen)) {
        BodyRender* r = &b->render;

        ImGui::ColorEdit3("Color", (float*)&r->atmosphere_color);
        ImGui::DragFloat("Thickness", &r->atmosphere_thickness, 0.005f, 0.01f, 1.0f, "%.3f");
        ImGui::DragFloat("Intensity", &r->atmosphere_intensity, 0.05f, 0.0f, 10.0f, "%.2f");
        ImGui::TextDisabled("Thickness is relative to planet radius");
    }

    if (b->render.emits_light && ImGui::CollapsingHeader("Light / Emission", ImGuiTreeNodeFlags_DefaultOpen)) {
        BodyRender* r = &b->render;
        r->temperature = fmaxf(r->temperature, 1000.0f);

        ImGui::DragFloat("Light Intensity", &r->light_intensity, 0.1f, 0.0f, FLT_MAX);
        ImGui::DragFloat("Emissive Strength", &r->emissive_strength, 0.01f, 0.0f, FLT_MAX);
        if (ImGui::SliderFloat("Temperature (K)", &r->temperature, 1000.0f, 40000.0f, "%.0f K")) {
            r->base_color = temperature_to_rgb(r->temperature);
        }
    }

    ImGui::PopID();
    ImGui::End();
}
