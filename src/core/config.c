#include "config.h"
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

EngineConfig g_config = {0};
bool parse_config_file(const char* path, EngineConfig* cfg);

bool config_load(const char* path)
{
    /* -------- WORLD DEFAULTS -------- */
    g_config.world.gravity_constant = 1.0f;
    g_config.world.bh_theta         = 0.7f;
    g_config.world.softening        = 0.01f;
    g_config.world.max_velocity     = 0.0f;
    g_config.world.max_bodies       = 1024;

    /* -------- SIM -------- */
    g_config.sim.screen_width  = 1280;
    g_config.sim.screen_height = 720;
    g_config.sim.time_scale    = 1.0f;
    g_config.sim.vsync         = true;
    g_config.sim.paused        = false;

    parse_config_file(path, &g_config);

    // Sanity check
    if (g_config.world.gravity_constant <= 0.0f)
        g_config.world.gravity_constant = 1.0f;

    if (g_config.world.bh_theta <= 0.0f || g_config.world.bh_theta >= 2.0f)
        g_config.world.bh_theta = 0.7f;

    if (g_config.world.softening < 0.0f)
        g_config.world.softening = 0.0f;

    if (g_config.sim.screen_width <= 0)  g_config.sim.screen_width = 1280;
    if (g_config.sim.screen_height <= 0) g_config.sim.screen_height = 720;

    return true;
}

static char* trim(char* s)
{
    while (isspace((unsigned char)*s)) s++;

    if (*s == 0) return s;

    char* end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end))
        *end-- = '\0';

    return s;
}

static bool parse_bool(const char* v)
{
    return strcmp(v, "1") == 0 ||
           strcmp(v, "true") == 0 ||
           strcmp(v, "yes") == 0 ||
           strcmp(v, "on") == 0;
}

bool parse_config_file(const char* path, EngineConfig* cfg)
{
    FILE* f = fopen(path, "r");
    if (!f)
        return false;

    char line[512];
    char section[64] = "";

    while (fgets(line, sizeof(line), f)) {
        char* s = trim(line);

        // Skip empty lines and comments
        if (*s == '\0' || *s == '#' || *s == ';')
            continue;

        // Section header
        if (*s == '[') {
            char* end = strchr(s, ']');
            if (!end) continue;

            *end = '\0';
            strncpy(section, s + 1, sizeof(section) - 1);
            section[sizeof(section) - 1] = '\0';
            continue;
        }

        // Key = Value
        char* eq = strchr(s, '=');
        if (!eq) continue;

        *eq = '\0';
        char* key = trim(s);
        char* val = trim(eq + 1);

        /* ---------------- WORLD ---------------- */
        if (strcmp(section, "world") == 0) {
            if (strcmp(key, "gravity_constant") == 0)
                cfg->world.gravity_constant = strtof(val, NULL);
            else if (strcmp(key, "bh_theta") == 0)
                cfg->world.bh_theta = strtof(val, NULL);
            else if (strcmp(key, "softening") == 0)
                cfg->world.softening = strtof(val, NULL);
            else if (strcmp(key, "max_velocity") == 0)
                cfg->world.max_velocity = strtof(val, NULL);
            else if (strcmp(key, "max_bodies") == 0)
                cfg->world.max_bodies = atoi(val);
        }

        /* ---------------- SIM ---------------- */
        else if (strcmp(section, "sim") == 0) {
            if (strcmp(key, "screen_width") == 0)
                cfg->sim.screen_width = atoi(val);
            else if (strcmp(key, "screen_height") == 0)
                cfg->sim.screen_height = atoi(val);
            else if (strcmp(key, "time_scale") == 0)
                cfg->sim.time_scale = strtof(val, NULL);
            else if (strcmp(key, "vsync") == 0)
                cfg->sim.vsync = parse_bool(val);
            else if (strcmp(key, "paused") == 0)
                cfg->sim.paused = parse_bool(val);
        }
    }

    fclose(f);
    return true;
}
