#include "orbit.h"
#include <math.h>
#include <raymath.h>

// Kepler's  First Law
Vector3 orbit_velocity(Vector3 position, Vector3 center, float central_mass, float G, bool clockwise)
{
    Vector3 radius = Vector3Subtract(position, center);
    float r = Vector3Length(radius);

    if (r <= 0.0f) return (Vector3){0};

    float speed = sqrtf(G * central_mass / r);

    Vector3 up = {0, 1, 0};
    Vector3 tangent = Vector3Normalize(Vector3CrossProduct(up, radius));

    if (!clockwise) {
        tangent = Vector3Negate(tangent);
    }

    return Vector3Scale(tangent, speed);
}
