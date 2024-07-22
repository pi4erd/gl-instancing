#pragma once

#include <fmt/format.h>

#define LOGLEVEL_DEBUG 3
#define LOGLEVEL_INFO 2
#define LOGLEVEL_WARN 1
#define LOGLEVEL_ERROR 0

#define LOG_LEVEL LOGLEVEL_DEBUG

#if LOG_LEVEL >= LOGLEVEL_DEBUG
    #define LOG_DEBUG(...) fmt::println(">\tDEBUG:\t{}", fmt::format(__VA_ARGS__))
#else
    #define LOG_DEBUG(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_INFO
    #define LOG_INFO(...) fmt::println(">>\tINFO:\t{}", fmt::format(__VA_ARGS__))
#else
    #define LOG_INFO(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_WARN
    #define LOG_WARN(...) fmt::println(">>>\tWARN:\t{}", fmt::format(__VA_ARGS__))
#else
    #define LOG_WARN(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_ERROR
    #define LOG_ERROR(...) fmt::println(">>>>\tERROR:\t{}", fmt::format(__VA_ARGS__))
#else
    #define LOG_ERROR(...)
#endif
