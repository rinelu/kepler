#pragma once
#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

Vector3 blackbody_to_rgb(float kelvin);
Color temperature_to_rgb(float kelvin);
Vector3 normalize_rgb(Vector3 c);
Vector3 srgb_to_linear(Vector3 c);

#ifdef __cplusplus
}
#endif
