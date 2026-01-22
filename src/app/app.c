#include "app.h"
#include <assert.h>
#include <raylib.h>

#include "../core/log.h"
#include "../world/body_factory.h"
#include "../render/celestial_render.h"

#include "../systems/octree_system.h"
#include "../systems/hierarchy_system.h"

bool app_init(App* app)
{
    assert(app);
    LOG_INFO("Initializing application...");

    if (!config_load(&app->config, "assets/config.cfg")) {
        LOG_ERROR("Failed to load configuration.");
        return false;
    }

    time_init(&app->time);
    time_set_scale(&app->time, app->config.sim.time_scale);
    time_set_paused(&app->time, app->config.sim.paused);

    app->services.world = world_create(app->config.world.max_bodies);
    assert(app->services.world);
    app_init_world(app->services.world);
    world_auto_assign_velocities(app->services.world, app->config.world.gravity_constant);
    LOG_INFO("World created with max bodies: %d", app->config.world.max_bodies);

    app->services.renderer = renderer_create(
        app->config.render.screen_width,
        app->config.render.screen_height,
        "Kepler",
        app->config.render.vsync);
    assert(app->services.renderer);
    celestial_render_init();
    LOG_INFO("Renderer initialized: %dx%d, VSync: %s", 
              app->config.render.screen_width, 
              app->config.render.screen_height, 
              app->config.render.vsync ? "enabled" : "disabled");

    camera_init(&app->camera, app->services.world);
    assert(&app->camera);
    LOG_INFO("Camera initialized");

    app->services.scheduler = scheduler_create();
    assert(app->services.scheduler);
    app_init_systems(app);
    LOG_INFO("Scheduler created.");

    app->should_quit = false;

    LOG_INFO("Application initialized successfully.");

    return true;
}

void app_init_world(World *world)
{
    WorldID sun = spawn_body(world, (Vector3){ 0, 0, 0 }, 5.0f, YELLOW);
    Body* b = world_get_body(world, sun);
    b->mass = 10.0f;

    WorldID earth = spawn_body(world, (Vector3){ 30, 0, 0 }, 1.0f, BLUE);
    Body* e = world_get_body(world, earth);
    e->mass = 1.0f;
}

void app_init_systems(App* app)
{
    Scheduler* s = app->services.scheduler;
    scheduler_add_system(s, "octree", octree_update, 100);
    scheduler_add_system(s, "hierarchy", hierarchy_update, 50);
}

void app_handle_input(App* app)
{
    if (IsKeyPressed(KEY_SPACE)) {
        time_set_paused(&app->time, !app->time.paused);
    }

    if (IsKeyPressed(KEY_N)) {
        time_step_once(&app->time);
    }

    if (IsKeyPressed(KEY_KP_1)) time_set_scale(&app->time, 1.0f);
    if (IsKeyPressed(KEY_KP_2)) time_set_scale(&app->time, 0.25f);
    if (IsKeyPressed(KEY_KP_3)) time_set_scale(&app->time, 4.0f);

    if (IsKeyPressed(KEY_F)) {
        time_set_fixed(&app->time, !app->time.use_fixed_timestep);
    }
}

void app_update(App* app)
{
    assert(app);
    time_begin_frame(&app->time);
    camera_update(&app->camera, app->time.real_dt);

    while (time_should_step(&app->time)) {
        scheduler_update(app->services.scheduler, app, app->time.dt);
        time_consume_step(&app->time);
    }

    time_end_frame(&app->time);
}

void app_render(App* app)
{
    renderer_begin_frame(app->services.renderer, app->camera.camera);
    renderer_draw_world(app->services.renderer, app->services.world, app->camera.camera);
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
