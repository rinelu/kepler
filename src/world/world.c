#include "world.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include "../dynamic_array.h"

static bool world_id_matches(const World* world, WorldID id)
{
    if (!world_id_is_valid(id)) return false;

    uint32_t index = world_id_index(id);
    if (index >= world->capacity) return false;

    const WorldSlot* slot = &world->items[index];
    return slot->alive && slot->generation == world_id_generation(id);
}

World* world_create(uint32_t max_bodies)
{
    assert(max_bodies > 0);

    World* world = (World*)calloc(1, sizeof(World));
    assert(world);

    world->items = (WorldSlot*)calloc(max_bodies, sizeof(WorldSlot));
    assert(world->items);

    for (uint32_t i = 0; i < max_bodies; ++i) {
        world->items[i].generation = 1;
    }

    world->capacity = max_bodies;
    world->count    = 0;

    return world;
}

void world_build_physics_views(World* world, GravBodyView* grav, PhysicsBodyView* phys, int* out_count)
{
    int i = 0;
    Body* b;
    world_foreach_body(world, b) {
        grav[i] = (GravBodyView){
            .position     = &b->position,
            .acceleration = &b->acceleration,
            .mass         = b->mass
        };

        phys[i] = (PhysicsBodyView){
            .position     = &b->position,
            .velocity     = &b->velocity,
            .acceleration = &b->acceleration,
            .mass         = &b->mass,
            .damping      = &b->damping
        };
        i++;
    }
    *out_count = i;
}

void world_destroy(World* world)
{
    if (!world) return;

    free(world->items);
    free(world);
}

void world_clear(World* world)
{
    for (uint32_t i = 0; i < world->capacity; ++i) {
        world->items[i].alive = false;
        world->items[i].generation++;
    }

    world->count = 0;
}

uint32_t world_body_count(const World* world)
{
    return world->count;
}

bool world_is_valid(WorldID id)
{
    return world_id_is_valid(id);
}

Body* world_get_body(World* world, WorldID id)
{
    if (!world_id_matches(world, id)) return NULL;
    return &world->items[world_id_index(id)].body;
}

WorldID world_add_body(World* world, const void* body_data)
{
    assert(body_data);

    for (uint32_t i = 0; i < world->capacity; ++i) {
        WorldSlot* slot = &world->items[i];

        if (slot->alive) continue;

        memcpy(&slot->body, body_data, sizeof(Body));
        slot->alive = true;

        WorldID id = world_id_make(i, slot->generation);
        world->count++;

        return id;
    }

    return WORLD_ID_INVALID;
}

void world_remove_body(World* world, WorldID id)
{
    if (!world_id_matches(world, id)) return;

    uint32_t index = world_id_index(id);
    WorldSlot* slot = &world->items[index];

    slot->alive = false;
    slot->generation++;
    world->count--;
}
