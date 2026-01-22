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

void world_destroy(World* world)
{
    if (!world) return;

    free(world->items);
    free(world);
}

void world_clear(World* world)
{
    assert(world);

    for (uint32_t i = 0; i < world->capacity; ++i) {
        world->items[i].alive = false;
        world->items[i].generation++;
    }

    world->count = 0;
}

void world_update(World* world, float dt)
{
    (void)world;
    (void)dt;

    /* TODO:
       Systems (gravity, orbit, etc.) operate directly on bodies.
       This exists as a hook for future world-level logic.
    */
}

uint32_t world_body_count(const World* world)
{
    assert(world);
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
    assert(world);
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
    assert(world);

    if (!world_id_matches(world, id)) return;

    uint32_t index = world_id_index(id);
    WorldSlot* slot = &world->items[index];

    slot->alive = false;
    slot->generation++;
    world->count--;
}

static Vector3 perpendicular(Vector3 v)
{
    Vector3 up = {0, 1, 0};
    if (fabsf(Vector3DotProduct(v, up)) > 0.99f)
        up = (Vector3){1, 0, 0};

    return Vector3Normalize(Vector3CrossProduct(up, v));
}

void world_auto_assign_velocities(World* world, float G)
{
    Body* a;
    world_foreach_body(world, a) {
        Vector3 gravity_dir = {0};
        float enclosed_mass = 0.0f;

        Body* b;
        world_foreach_body(world, b) {
            if (a == b) continue;
            if (b->mass <= 0) continue;

            Vector3 d = Vector3Subtract(b->position, a->position);
            float r2 = Vector3LengthSqr(d);
            if (r2 <= 0.0001f) continue;

            gravity_dir = Vector3Add(gravity_dir, Vector3Normalize(d));
            enclosed_mass += b->mass;
        }

        if (enclosed_mass <= 0.0f) continue;

        float r = Vector3Length(gravity_dir);
        Vector3 dir = Vector3Normalize(gravity_dir);
        Vector3 tangent = perpendicular(dir);
        float speed = sqrtf(G * enclosed_mass / fmaxf(r, 0.01f));

        a->velocity = Vector3Scale(tangent, speed);
    }
}

