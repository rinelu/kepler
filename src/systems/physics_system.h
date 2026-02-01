#pragma once
#include <stddef.h>
#include <raylib.h>
#include "octree/octree.h"
#include "world/body.h"

void physics_integrate( PhysicsBodyView* bodies, int count, float dt);
void physics_step(GravBodyView* grav, PhysicsBodyView* phys, int count, float dt, const OctreeConfig* cfg);
void physics_system_update(float dt);
