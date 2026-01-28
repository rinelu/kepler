#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#if defined(__GNUC__) || defined(__clang__)
#   ifdef __MINGW_PRINTF_FORMAT
#       define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#   else
#       define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#   endif
#else
#   define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typedef enum {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    TODO,
    UNREACHABLE
} LogLevel;

#ifdef NDEBUG
static LogLevel min_level = INFO;
#else
static LogLevel min_level = TRACE;
#endif

#ifndef LOG_THREAD_SAFE
#define LOG_THREAD_SAFE 1
#endif

#if LOG_THREAD_SAFE
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static const char* level_name(LogLevel lvl) {
    switch (lvl) {
        case TRACE: return "TRACE";
        case DEBUG: return "DEBUG";
        case INFO:  return "INFO ";
        case WARN:  return "WARN ";
        case ERROR: return "ERROR";
        case CRITICAL: return "CRIT ";
        case TODO: return "TODO ";
        case UNREACHABLE: return "UNREACHABLE";
        default: return "UNKN";
    }
}

static int use_color = 1;

static const char* level_color(LogLevel lvl) {
    if (!use_color) return "";

    switch (lvl) {
        case TRACE: return "\033[90m";
        case DEBUG:
        case TODO: return "\033[36m";
        case INFO: return "\033[32m";
        case WARN: return "\033[33m";
        case ERROR: return "\033[31m";
        case CRITICAL:
        case UNREACHABLE: return "\033[41m\033[97m";
        default: return "";
    }
}

typedef void (*LogHandler)(LogLevel, const char*);


static void default_handler(LogLevel lvl, const char* message) {
    FILE* out = (lvl >= WARN) ? stderr : stdout;

    if (use_color) {
        fprintf(out,"%s%-3s\033[0m %s\n", level_color(lvl), level_name(lvl), message);
    } else {
        fprintf(out, "%-3s %s\n", level_name(lvl), message);
    }

    fflush(out);
}


static LogHandler handler = default_handler;

static void set_handler(LogHandler h) {
    handler = h ? h : default_handler;
}

static void set_level(LogLevel lvl) {
    min_level = lvl;
}

static void log_write(LogLevel lvl, const char* fmt, ...) PRINTF_FORMAT(2, 3);

static void log_write(LogLevel lvl, const char* fmt, ...) {
    if (lvl < min_level || !handler) return;

#if LOG_THREAD_SAFE
    pthread_mutex_lock(&log_mutex);
#endif

    va_list args;
    va_start(args, fmt);
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    handler(lvl, buf);

#if LOG_THREAD_SAFE
    pthread_mutex_unlock(&log_mutex);
#endif
}

#define LOG_TRACE(...) log_write(TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) log_write(DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  log_write(INFO, __VA_ARGS__)
#define LOG_WARN(...)  log_write(WARN, __VA_ARGS__)
#define LOG_ERROR(...) log_write(ERROR, __VA_ARGS__)
#define LOG_CRITICAL(...) log_write(CRITICAL, __VA_ARGS__)
#define LOG_THROW(...) do { LOG_ERROR(__VA_ARGS__); abort(); } while(0)
#define LOG_TODO(...) do { log_write(TODO, __VA_ARGS__); abort(); } while(0)
#define LOG_UNREACHABLE(...) do { log_write(UNREACHABLE, __VA_ARGS__); abort(); } while(0)
