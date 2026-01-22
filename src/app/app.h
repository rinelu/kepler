#pragma once
#include <stdbool.h>
#include "../world/world.h"
#include "../core/config.h"
#include "../systems/scheduler.h"
#include "../time/time.h"
#include "../render/renderer.h"
#include "../camera/camera.h"

typedef struct App App;

typedef struct {
    World* world;
    Renderer* renderer;
    Scheduler* scheduler;
} AppServices;

typedef struct App {
    KeplerConfig config;
    AppServices services;
    Time time;
    CameraState camera;

    bool should_quit;
} App;

bool app_init(App* app);

void app_init_world(World* world);
void app_init_systems(App* app);

void app_update(App* app);
void app_handle_input(App* app);
void app_render(App* app);
void app_shutdown(App* app);
