#include "scheduler.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../dynamic_array.h"

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

Scheduler* scheduler_create(void) {
    Scheduler* scheduler = calloc(1, sizeof(Scheduler));
    assert(scheduler);

    scheduler->items    = NULL;
    scheduler->count    = 0;
    scheduler->capacity = 0;

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

    ScheduledSystem sys = {
        .name    = name,
        .update  = update_fn,
        .order   = order,
        .enabled = 1,
    };
    da_append(scheduler, sys);

    // Keep execution deterministic
    qsort(
        scheduler->items,
        scheduler->count,
        sizeof(ScheduledSystem),
        system_compare
    );
}

void scheduler_update(Scheduler* scheduler, App* app, float dt) {
    assert(scheduler);
    assert(app);

    da_foreach(ScheduledSystem, sys, scheduler) {
        if (!sys->enabled) continue;
        sys->update(app, dt);
    }
}

void scheduler_set_enabled(Scheduler* scheduler, const char* name, int enabled)
{
    assert(scheduler);
    assert(name);

    da_foreach(ScheduledSystem, sys, scheduler) {
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
