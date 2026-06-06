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

#define VN_FATAL(fmt, ...)                                                            \
    do {                                                                              \
        std::cerr << std::format(                                                     \
            "[FATAL]\nFile: {}\nLine: {}\n" fmt "\n", __FILE__, __LINE__, __VA_ARGS__ \
        );                                                                            \
        VN_DEBUG_BREAK();                                                             \
        std::abort();                                                                 \
    } while (0)

#ifdef NDEBUG
    #define VN_ASSERT(...) ((void)0)

#else
    #define VN_ASSERT(cond, fmt, ...)      \
        do {                               \
            if (!(cond)) {                 \
                std::cerr << std::format(  \
                    "[ASSERT] {}\n"        \
                    "File: {}\n"           \
                    "Line: {}\n" fmt "\n", \
                    #cond,                 \
                    __FILE__,              \
                    __LINE__,              \
                    ##__VA_ARGS__          \
                );                         \
                VN_DEBUG_BREAK();          \
                std::abort();              \
            }                              \
        } while (0)

#endif