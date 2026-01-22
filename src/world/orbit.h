#pragma once
#include <raylib.h>
#include <stdbool.h>

Vector3 orbit_velocity(Vector3 position, Vector3 center, float central_mass, float G, bool clockwise);
