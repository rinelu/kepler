#include "hierarchy_system.h"
#include "core/engine.h"

void hierarchy_update(float dt)
{
    World* world = engine()->world;

    Body* b;
    world_foreach_body(world, b) {
        if (!world_is_valid(b->parent)) continue;

        Body* parent = world_get_body(world, b->parent);
        if (!parent) continue;
    }
}
