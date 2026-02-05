#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void ImGuiLayer_Init(void);
void ImGuiLayer_Begin(void);
bool ImGuiLayer_CaptureInput(void);
void ImGuiLayer_Draw(void);
void ImGuiLayer_End(void);
void ImGuiLayer_Shutdown(void);

#ifdef __cplusplus
}
#endif
