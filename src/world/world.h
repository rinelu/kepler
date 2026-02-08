#pragma once
#include "body.h"
#include "world_ids.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  Body body;
  uint32_t generation;
  bool alive;
} WorldSlot;

typedef struct {
    WorldSlot* items;
    uint32_t*  alive_indices;
    uint32_t   alive_count;

    uint32_t capacity;
    uint32_t count;

    uint64_t revision;
} World;


World *world_create(uint32_t max_bodies);
void world_build_physics_views(World *world, GravBodyView *grav,
                               PhysicsBodyView *phys, int *out_count);
void world_destroy(World *world);
void world_clear(World *world);

uint32_t world_body_count(const World *world);

bool world_is_valid(WorldID id);
Body *world_get_body(World *world, WorldID id);

WorldID world_add_body(World *world, const void* body_data);
void world_remove_body(World *world, WorldID id);

#define world_foreach_body(world, body_ptr)                                       \
    for (uint32_t _i = 0; _i < (world)->alive_count &&                            \
        ((body_ptr) = &(world)->items[(world)->alive_indices[_i]].body, 1); _i++)

#define world_foreach_body_i(world, i, body_ptr)                               \
  for (uint32_t i = 0; i < (world)->capacity; ++i)                             \
    if ((world)->items[i].alive && ((body_ptr) = &(world)->items[i].body))

#ifdef __cplusplus
}
#endif
