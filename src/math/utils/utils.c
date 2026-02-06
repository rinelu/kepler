#include "utils.h"

Vector3 blackbody_to_rgb(float kelvin)
{
    float t = kelvin / 100.0f;
    float r, g, b;

    // Red
    if (t <= 66.0f) r = 1.0f;
    else {
        r = t - 60.0f;
        r = 329.698727446f * powf(r, -0.1332047592f) / 255.0f;
    }

    // Green
    if (t <= 66.0f) {
        g = 99.4708025861f * logf(t) - 161.1195681661f;
        g /= 255.0f;
    } else {
        g = t - 60.0f;
        g = 288.1221695283f * powf(g, -0.0755148492f) / 255.0f;
    }

    // Blue
    if (t >= 66.0f) b = 1.0f;
    else if (t <= 19.0f) b = 0.0f;
    else {
        b = t - 10.0f;
        b = 138.5177312231f * logf(b) - 305.0447927307f;
        b /= 255.0f;
    }

    return (Vector3){ Clamp(r, 0.0f, 1.0f), Clamp(g, 0.0f, 1.0f), Clamp(b, 0.0f, 1.0f) };
}

Color temperature_to_rgb(float kelvin)
{
    kelvin = Clamp(kelvin, 1000.0f, 40000.f);
    Vector3 c = blackbody_to_rgb(kelvin);

    return (Color){
        (unsigned char)(c.x * 255.0f),
        (unsigned char)(c.y * 255.0f),
        (unsigned char)(c.z * 255.0f),
        255
    };
}

Vector3 normalize_rgb(Vector3 c)
{
    float maxc = fmaxf(c.x, fmaxf(c.y, c.z));
    if (maxc > 0.0f) { c.x /= maxc; c.y /= maxc; c.z /= maxc; }
    return c;
}

Vector3 srgb_to_linear(Vector3 c)
{
    return (Vector3){ powf(c.x, 2.2f), powf(c.y, 2.2f), powf(c.z, 2.2f) };
}
