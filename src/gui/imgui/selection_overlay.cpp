#include "engine/engine.h"
#include "world/world.h"
#include <raylib.h>
#include <imgui.h>

void selection_overlay_draw(World* world)
{
    Engine* e = engine();
    if (!world_id_is_valid(e->selected_body)) return;

    Body* body = world_get_body(world, e->selected_body);
    if (!body || !body->visible) return;

    Vector2 screen = GetWorldToScreen(body->position, e->renderer->ctx.camera);
    screen.y -= 12.0f;

    ImGui::SetNextWindowPos(ImVec2(screen.x, screen.y), ImGuiCond_Always, ImVec2(0.5f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("##selection_label", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs);

    ImGui::Text("%s", body->name);
    ImGui::End();
}
