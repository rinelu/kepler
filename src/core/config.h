#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct {
    struct {
        float gravity_constant;
        int   max_bodies;
    } world;

    struct {
        int  screen_width;
        int  screen_height;
        bool vsync;
    } render;

    struct {
        float time_scale;
        bool  paused;
    } sim;
} KeplerConfig;

static inline char* cfg_trim(char* s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;

    char* end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

static inline bool cfg_parse_bool(const char* s, bool* out) {
    if (!s || !out) return false;

    if (!strcmp(s, "true") || !strcmp(s, "1"))  { *out = true;  return true; }
    if (!strcmp(s, "false") || !strcmp(s, "0")) { *out = false; return true; }

    return false;
}

static inline bool config_load(KeplerConfig* out, const char* path) {
    if (!out || !path) {
        return false;
    }

    // defaults
    out->world.gravity_constant = 1.0f;
    out->world.max_bodies       = 1024;

    out->render.screen_width  = 1280;
    out->render.screen_height = 720;
    out->render.vsync         = true;

    out->sim.time_scale = 1.0f;
    out->sim.paused     = false;

    FILE* f = fopen(path, "r");
    if (!f) return true;

    char line[256];
    char section[32] = "";

    while (fgets(line, sizeof(line), f)) {
        char* s = cfg_trim(line);

        /* skip empty lines and comments */
        if (*s == '\0' || *s == '#' || *s == ';') {
            continue;
        }

        /* section header */
        if (*s == '[') {
            char* end = strchr(s, ']');
            if (!end) continue;

            *end = '\0';
            strncpy(section, s + 1, sizeof(section) - 1);
            section[sizeof(section) - 1] = '\0';
            continue;
        }

        /* key = value */
        char* eq = strchr(s, '=');
        if (!eq) continue;

        *eq = '\0';
        char* key   = cfg_trim(s);
        char* value = cfg_trim(eq + 1);

        /* ---- world ---- */
        if (!strcmp(section, "world")) {
            if (!strcmp(key, "gravity_constant")) {
                out->world.gravity_constant = (float)atof(value);
            } else if (!strcmp(key, "max_bodies")) {
                out->world.max_bodies = atoi(value);
            }
        }

        /* ---- render ---- */
        else if (!strcmp(section, "render")) {
            if (!strcmp(key, "screen_width")) {
                out->render.screen_width = atoi(value);
            } else if (!strcmp(key, "screen_height")) {
                out->render.screen_height = atoi(value);
            } else if (!strcmp(key, "vsync")) {
                cfg_parse_bool(value, &out->render.vsync);
            }
        }

        /* ---- sim ---- */
        else if (!strcmp(section, "sim")) {
            if (!strcmp(key, "time_scale")) {
                out->sim.time_scale = (float)atof(value);
            } else if (!strcmp(key, "paused")) {
                cfg_parse_bool(value, &out->sim.paused);
            }
        }
    }

    fclose(f);

    /* ---- validation / clamping ---- */
    if (out->world.max_bodies <= 0) out->world.max_bodies = 1;
    if (out->sim.time_scale < 0.0f) out->sim.time_scale = 0.0f;

    return true;
}
