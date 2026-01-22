#pragma once
#include <stdint.h>

typedef struct Scheduler Scheduler;
typedef struct App App;

typedef void (*UpdateFn)(App* app, float dt);

Scheduler* scheduler_create(void);
void       scheduler_destroy(Scheduler* scheduler);

void scheduler_add_system(Scheduler* scheduler, const char* name, UpdateFn update_fn, int order);

void scheduler_update(Scheduler* scheduler, App* app, float dt);
void scheduler_set_enabled(Scheduler* scheduler, const char* name, int enabled);
void scheduler_clear(Scheduler* scheduler);
