#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void ImGuiLayer_Init(void);
void ImGuiLayer_Begin(void);
bool ImGuiLayer_CaptureInput(void);
void ImGuiLayer_Draw(void);
void ImGuiLayer_End(void);
void ImGuiLayer_Shutdown(void);

void NuklearLayer_Init(void);
bool NuklearLayer_CaptureInput(void);
void NuklearLayer_Begin(void);
void NuklearLayer_Draw(void);
void NuklearLayer_End(void);
void NuklearLayer_Shutdown(void);

#ifdef __cplusplus
}
#endif
