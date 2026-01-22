#pragma once

#include <stdint.h>
#include <stdbool.h>

/*
    WorldID
    32-bit stable handle:
    - index      : lower bits
    - generation : upper bits
*/

typedef struct {
    uint32_t value;
} WorldID;

#define WORLD_ID_INDEX_BITS       20
#define WORLD_ID_GENERATION_BITS  12

#define WORLD_ID_INDEX_MASK       ((1u << WORLD_ID_INDEX_BITS) - 1u)
#define WORLD_ID_GENERATION_MASK  ((1u << WORLD_ID_GENERATION_BITS) - 1u)

#define WORLD_ID_INVALID ((WorldID){0})

static inline WorldID world_id_make(uint32_t index, uint32_t generation) {
    WorldID id;
    id.value = (generation << WORLD_ID_INDEX_BITS) | (index & WORLD_ID_INDEX_MASK);
    return id;
}

static inline uint32_t world_id_index(WorldID id) {
    return id.value & WORLD_ID_INDEX_MASK;
}

static inline uint32_t world_id_generation(WorldID id) {
    return id.value >> WORLD_ID_INDEX_BITS;
}

static inline bool world_id_is_valid(WorldID id) {
    return id.value != 0;
}

static inline bool world_id_equal(WorldID a, WorldID b) {
    return a.value == b.value;
}
