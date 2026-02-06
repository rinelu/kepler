#include "scheduler.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
    const char* name;
    UpdateFn    update;
    int         order;
    int         enabled;
} ScheduledSystem;

struct Scheduler {
    ScheduledSystem* items;
    size_t count;
    size_t capacity;
};

static int system_compare(const void* a, const void* b)
{
    const ScheduledSystem* sa = (const ScheduledSystem*)a;
    const ScheduledSystem* sb = (const ScheduledSystem*)b;

    // Lower order runs first
    return sa->order - sb->order;
}

static void scheduler_grow_if_needed(Scheduler* scheduler)
{
    if (scheduler->count < scheduler->capacity) return;

    size_t new_capacity = scheduler->capacity ? scheduler->capacity * 2 : 8;
    ScheduledSystem* new_items = realloc(scheduler->items, new_capacity * sizeof(ScheduledSystem));
    assert(new_items);

    scheduler->items    = new_items;
    scheduler->capacity = new_capacity;
}

Scheduler* scheduler_create(void)
{
    Scheduler* scheduler = calloc(1, sizeof(Scheduler));
    assert(scheduler);
    return scheduler;
}

void scheduler_destroy(Scheduler* scheduler)
{
    if (!scheduler) return;

    free(scheduler->items);
    free(scheduler);
}

void scheduler_add_system(Scheduler* scheduler, const char* name, UpdateFn update_fn, int order)
{
    assert(scheduler);
    assert(update_fn);
    assert(name);

    scheduler_grow_if_needed(scheduler);

    scheduler->items[scheduler->count++] = (ScheduledSystem){
        .name    = name,
        .update  = update_fn,
        .order   = order,
        .enabled = 1,
    };

    qsort(scheduler->items, scheduler->count, sizeof(ScheduledSystem), system_compare);
}

void scheduler_update(Scheduler* scheduler, float dt)
{
    assert(scheduler);

    for (size_t i = 0; i < scheduler->count; ++i) {
        ScheduledSystem* sys = &scheduler->items[i];
        if (!sys->enabled) continue;
        sys->update(dt);
    }
}

void scheduler_set_enabled(Scheduler* scheduler, const char* name, int enabled)
{
    assert(scheduler);
    assert(name);

    for (size_t i = 0; i < scheduler->count; ++i) {
        ScheduledSystem* sys = &scheduler->items[i];
        if (strcmp(sys->name, name) == 0) {
            sys->enabled = enabled ? 1 : 0;
            return;
        }
    }
}

void scheduler_clear(Scheduler* scheduler)
{
    assert(scheduler);
    scheduler->count = 0;
}
