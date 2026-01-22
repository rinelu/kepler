#pragma once

#include "world_ids.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>
#include "body.h"

typedef struct {
    Body     body;
    uint32_t generation;
    bool     alive;
} WorldSlot;

typedef struct {
    WorldSlot* items;
    uint32_t   capacity;
    uint32_t   count;
} World;

World *world_create(uint32_t max_bodies);
void world_destroy(World *world);
void world_clear(World *world);

void world_update(World *world, float dt);

uint32_t world_body_count(const World *world);

bool world_is_valid(WorldID id);
Body* world_get_body(World *world, WorldID id);

WorldID world_add_body(World *world, const void *body_data);
void world_remove_body(World *world, WorldID id);

void world_auto_assign_velocities(World* world, float G);

#define world_foreach_body(world, body_ptr)                                   \
    for (uint32_t _i = 0; _i < (world)->capacity; ++_i)                       \
        if ((world)->items[_i].alive &&                                       \
            ((body_ptr) = &(world)->items[_i].body))

#define world_foreach_body_i(world, i, body_ptr)                              \
    for (uint32_t i = 0; i < (world)->capacity; ++i)                          \
        if ((world)->items[i].alive &&                                        \
            ((body_ptr) = &(world)->items[i].body))
