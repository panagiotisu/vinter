#pragma once

#include <cstdlib>
#include <format>
#include <iostream>

#ifdef _MSC_VER
    #define VN_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define VN_DEBUG_BREAK() __builtin_trap()
#else
    #define VN_DEBUG_BREAK() std::abort()
#endif

// ANSI color codes for red text
#define VN_RED   "\033[31m"
#define VN_RESET "\033[0m"

#define VN_FATAL(fmt, ...)                   \
    do {                                     \
        std::cerr << VN_RED                  \
                  << std::format(            \
                         "[FATAL] " fmt "\n" \
                         "File: {}\n"        \
                         "Line: {}\n",       \
                         __VA_ARGS__,        \
                         __FILE__,           \
                         __LINE__            \
                     )                       \
                  << VN_RESET;               \
        VN_DEBUG_BREAK();                    \
        std::abort();                        \
    } while (0)

#ifdef NDEBUG
    #define VN_ASSERT(...) ((void)0)

#else
    #define VN_ASSERT(cond, fmt, ...)                 \
        do {                                          \
            if (!(cond)) {                            \
                std::cerr << VN_RED                   \
                          << std::format(             \
                                 "[ASSERT] " fmt "\n" \
                                 "Condition: {}\n"    \
                                 "File: {}\n"         \
                                 "Line: {}\n",        \
                                 ##__VA_ARGS__,       \
                                 #cond,               \
                                 __FILE__,            \
                                 __LINE__             \
                             )                        \
                          << VN_RESET;                \
                VN_DEBUG_BREAK();                     \
                std::abort();                         \
            }                                         \
        } while (0)

#endif