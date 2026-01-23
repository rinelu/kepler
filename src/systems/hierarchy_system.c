#include "hierarchy_system.h"

void hierarchy_update(App* app, float dt)
{
    World* world = app->services.world;

    Body* b;
    world_foreach_body(world, b) {
        if (!world_is_valid(b->parent)) continue;

        Body* parent = world_get_body(world, b->parent);
        if (!parent) continue;

        b->position = Vector3Add(parent->position, b->local_position);
    }
}
