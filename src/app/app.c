#include "app.h"
#include <assert.h>
#include <raylib.h>

#include "camera/camera.h"
#include "core/config.h"
#include "core/engine.h"
#include "core/log.h"
// #include "systems/relax_system.h"
#include "world/body_factory.h"
#include "render/renderer.h"
#include "systems/hierarchy_system.h"
#include "systems/physics_system.h"
#include "world/orbit/orbit.h"
#include "world/world.h"

bool app_init()
{
    LOG_INFO("Initializing application...");
    Engine* e       = engine();
    EngineConfig* c = config();

    if (!config_load("assets/config.cfg")) {
        LOG_ERROR("Failed to load configuration.");
        return false;
    }

    time_init(&e->time);

    e->world = world_create(config()->world.max_bodies);
    assert(e->world);
    LOG_INFO("World created with max bodies: %d", config()->world.max_bodies);

    e->renderer = renderer_create(
        config()->sim.screen_width,
        config()->sim.screen_height,
        "Kepler",
        config()->sim.vsync);
    assert(e->renderer);
    e->renderer->ctx = renderer_build_context(&e->camera);
    LOG_INFO("Renderer initialized: %dx%d, VSync: %s", 
              config()->sim.screen_width, 
              config()->sim.screen_height, 
              config()->sim.vsync ? "enabled" : "disabled");

    camera_init(&e->camera, e->world);
    assert(&e->camera);
    LOG_INFO("Camera initialized");

    e->scheduler = scheduler_create();
    assert(e->scheduler);
    LOG_INFO("Scheduler created.");

    predict_state_init(&e->predict);
    assert(&e->predict);
    LOG_INFO("World prediction initialized.");

    LOG_INFO("Application initialized successfully.");

    app_create_planets(e->world);
    app_init_systems(e->scheduler);

    // orbit_plummer(e->world, 50);
    // orbit_virial_rotating(e->world);
    orbit_keplerian(e->world);
    // relaxation_init();

    return true;
}

bool app_should_close() { return WindowShouldClose(); }

void app_create_planets(World* world)
{
    WorldID sun = spawn_body(world, &(BodyParam){
        .name     = "Sun",
        .density  = DENSITY_STAR,
        .mass     = 100.0f,
        .position = {0, 0, 0},
        .base_color = YELLOW
    });
    body_make_star(world_get_body(world, sun), 10000, 4.0f, 100);

    spawn_body(world, &(BodyParam){
        .name     = "Earth",
        .mass     = 1.0f,
        .density  = DENSITY_ROCK,
        .position = {45, 0.2f, 0},
        .base_color = BLUE
    });
    spawn_body(world, &(BodyParam){
        .name     = "Mars",
        .mass     = .5f,
        .density  = DENSITY_ROCK,
        .position = {70, 0, 0.1f},
        .base_color = RED
    });
    spawn_body(world, &(BodyParam){
        .name     = "Jupiter",
        .mass     = 5.0f,
        .density  = DENSITY_ROCK,
        .position = {120, 0, 0.4f},
        .base_color = RED
    });
    camera_set_follow(&engine()->camera, sun);
}

void app_init_systems(Scheduler* s)
{
    // scheduler_add_system(s, "relax",   relaxation_update, 10);
    scheduler_add_system(s, "hierarchy", hierarchy_update, 50);
    scheduler_add_system(s, "physics", physics_system_update, 100);
}

void app_handle_input()
{
    Engine* e = engine();
    if (IsKeyPressed(KEY_SPACE)) {
        time_set_paused(&e->time, !e->time.paused);
    }

    if (IsKeyPressed(KEY_N)) {
        time_step_once(&e->time);
    }
 
    if (IsKeyPressed(KEY_F)) {
        time_set_fixed(&e->time, !e->time.use_fixed_timestep);
    }
}

void app_update()
{
    Engine* e = engine();
    time_begin_frame(&e->time);
    camera_update(&e->camera, e->time.real_dt);
    renderer_update_context(&e->renderer->ctx, &e->camera);

    while (time_should_step(&e->time)) {
        scheduler_update(e->scheduler, e->time.fixed_dt);
        time_consume_step(&e->time);
    }

    predict_state_update(&e->predict, e->world);
    time_end_frame(&e->time);
}

void app_render()
{
    Engine* e = engine();
    renderer_begin_frame(e->renderer);
    renderer_render_world(e->renderer, e->world, &e->predict);
    renderer_render_gui(e->renderer);
    renderer_end_frame(e->renderer);
}

void app_shutdown()
{
    Engine* e = engine();
    LOG_INFO("Shutting down application...");

    predict_state_shutdown(&e->predict);
    scheduler_destroy(e->scheduler);
    world_destroy(e->world);
    renderer_destroy(e->renderer);

    LOG_INFO("Application shutdown successfully.");
}
