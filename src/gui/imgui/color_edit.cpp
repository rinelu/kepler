#include "color_edit.h"
#include "imgui.h"

bool ImGui_ColorEdit3_Color(const char* label, Color* color)
{
    float col[3] = { color->r / 255.0f, color->g / 255.0f, color->b / 255.0f };
    bool changed = ImGui::ColorEdit3(label, col);

    if (changed) {
        color->r = (unsigned char)(col[0] * 255.0f);
        color->g = (unsigned char)(col[1] * 255.0f);
        color->b = (unsigned char)(col[2] * 255.0f);
    }

    return changed;
}

bool ImGui_ColorEdit4_Color(const char* label, Color* color)
{
    float col[4] = { color->r / 255.0f, color->g / 255.0f, color->b / 255.0f, color->a / 255.0f };
    bool changed = ImGui::ColorEdit4(label, col);

    if (changed) {
        color->r = (unsigned char)(col[0] * 255.0f);
        color->g = (unsigned char)(col[1] * 255.0f);
        color->b = (unsigned char)(col[2] * 255.0f);
        color->a = (unsigned char)(col[3] * 255.0f);
    }

    return changed;
}
