#include "app.h"
#include <assert.h>
#include <raylib.h>

#include "../core/log.h"
#include "../world/body_factory.h"
#include "../render/celestial_render.h"

#include "../systems/octree_system.h"
#include "../systems/hierarchy_system.h"

#include "render/renderer.h"

// TODO: Make AppServices become a global variable :0
bool app_init(App* app)
{
    assert(app);
    LOG_INFO("Initializing application...");

    if (!config_load("assets/config.cfg")) {
        LOG_ERROR("Failed to load configuration.");
        return false;
    }

    time_init(&app->time);

    app->services.world = world_create(config()->world.max_bodies);
    assert(app->services.world);
    LOG_INFO("World created with max bodies: %d", config()->world.max_bodies);

    app->services.renderer = renderer_create(
        config()->sim.screen_width,
        config()->sim.screen_height,
        "Kepler",
        config()->sim.vsync);
    assert(app->services.renderer);
    app->services.renderer->ctx = renderer_build_context(&app->camera);
    celestial_render_init();
    LOG_INFO("Renderer initialized: %dx%d, VSync: %s", 
              config()->sim.screen_width, 
              config()->sim.screen_height, 
              config()->sim.vsync ? "enabled" : "disabled");

    camera_init(&app->camera, app->services.world);
    assert(&app->camera);
    LOG_INFO("Camera initialized");

    app->services.scheduler = scheduler_create();
    assert(app->services.scheduler);
    LOG_INFO("Scheduler created.");

    app->should_quit = false;

    LOG_INFO("Application initialized successfully.");

    app_init_world(app);
    app_init_systems(app);
    world_auto_assign_velocities(app->services.world, config()->world.gravity_constant, 0.0f, true);

    return true;
}

void app_init_world(App* app)
{
    World *world = app->services.world;
    spawn_body(world, &(SpawnBodyDesc){
        .name     = "Sun",
        .mass     = 10.0f,
        .density  = DENSITY_STAR,
        .position = {0, 0, 0},
        .velocity = {0, 0, 0},
        .base_color = YELLOW
    });
    spawn_body(world, &(SpawnBodyDesc){
        .name     = "Earth",
        .mass     = 1.0f,
        .density  = DENSITY_ROCK,
        .position = {20, 0, 0},
        .base_color = BLUE
    });
}

void app_init_systems(App* app)
{
    Scheduler* s = app->services.scheduler;
    scheduler_add_system(s, "hierarchy", hierarchy_update, 50);
    scheduler_add_system(s, "octree", octree_update, 100);
}

void app_handle_input(App* app)
{
    if (IsKeyPressed(KEY_SPACE)) {
        time_set_paused(&app->time, !app->time.paused);
    }

    if (IsKeyPressed(KEY_N)) {
        time_step_once(&app->time);
    }
 
    if (IsKeyPressed(KEY_F)) {
        time_set_fixed(&app->time, !app->time.use_fixed_timestep);
    }
}

void app_update(App* app)
{
    assert(app);
    time_begin_frame(&app->time);
    camera_update(&app->camera, app->time.real_dt);
    renderer_update_context(&app->services.renderer->ctx, &app->camera);

    while (time_should_step(&app->time)) {
        scheduler_update(app->services.scheduler, app, app->time.fixed_dt);
        time_consume_step(&app->time);
    }

    time_end_frame(&app->time);
}

void app_render(App* app)
{
    renderer_begin_frame(app->services.renderer);
    renderer_render_world(app->services.renderer, app->services.world);
    renderer_render_gui(app->services.renderer);
    renderer_end_frame(app->services.renderer);
}

void app_shutdown(App* app)
{
    assert(app);

    LOG_INFO("Shutting down application...");

    scheduler_destroy(app->services.scheduler);
    celestial_render_shutdown();
    world_destroy(app->services.world);
    renderer_destroy(app->services.renderer);

    LOG_INFO("Application shutdown successfully.");
}
