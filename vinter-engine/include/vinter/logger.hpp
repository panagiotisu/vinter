#pragma once

#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string_view>
#include <utility>

#ifdef _MSC_VER
    #define VN_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define VN_DEBUG_BREAK() __builtin_trap()
#else
    #define VN_DEBUG_BREAK() std::abort()
#endif

namespace vn {

    class Logger {
    public:
        enum class Level : std::uint8_t {
            Debug,
            Info,
            Warning,
            Error,
        };

        static void set_level_filter(Level level) {
            m_level_filter = level;
        }

        static void enable_colored_logs(bool enabled) {
            m_colored_logs = enabled;
        }

        template <typename... Args>
        static void debug(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Debug,
                "[DEBUG]",
                ConsoleColor::Gray,
                std::cout,
                fmt,
                std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void info(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Info,
                "[INFO]",
                ConsoleColor::Green,
                std::cout,
                fmt,
                std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void warning(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Warning,
                "[WARNING]",
                ConsoleColor::Yellow,
                std::cout,
                fmt,
                std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void error(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Error,
                "[ERROR]",
                ConsoleColor::Red,
                std::cerr,
                fmt,
                std::forward<Args>(args)...);
        }

        template <typename... Args>
        [[noreturn]]
        static void
        fatal(const char* file, int line, std::format_string<Args...> fmt, Args&&... args) {
            if (m_colored_logs) {
                std::cerr << ConsoleColor::Red;
            }

            std::cerr << "[FATAL] " << std::format(fmt, std::forward<Args>(args)...)
                      << "\nFile: " << file << "\nLine: " << line << '\n';

            if (m_colored_logs) {
                std::cerr << ConsoleColor::Reset;
            }

            std::cerr.flush();

            VN_DEBUG_BREAK();
            std::abort();
        }

        template <typename... Args>
        [[noreturn]]
        static void assertion_failed(
            const char* condition,
            const char* file,
            int line,
            std::format_string<Args...> fmt,
            Args&&... args
        ) {
            fatal(
                file,
                line,
                "Assertion failed.\nCondition: {}\n{}",
                condition,
                std::format(fmt, std::forward<Args>(args)...)
            );
        }

    private:
        struct ConsoleColor {
            static constexpr const char* Reset = "\033[0m";
            static constexpr const char* Gray = "\033[90m";
            static constexpr const char* Green = "\033[32m";
            static constexpr const char* Yellow = "\033[33m";
            static constexpr const char* Red = "\033[31m";
        };

        template <typename... Args>
        static void
        log(Level level,
            std::string_view label,
            const char* color,
            std::ostream& out,
            std::format_string<Args...> fmt,
            Args&&... args) {
            if (level < m_level_filter) {
                return;
            }

            if (m_colored_logs) {
                out << color;
            }

            out << label << ' ' << std::format(fmt, std::forward<Args>(args)...);

            if (m_colored_logs) {
                out << ConsoleColor::Reset;
            }

            out << '\n';
        }

        inline static Level m_level_filter = Level::Debug;
        inline static bool m_colored_logs = true;
    };

} // namespace vn

#ifdef NDEBUG

    #define VN_DEBUG(...)   ((void)0)
    #define VN_INFO(...)    ((void)0)
    #define VN_WARNING(...) ((void)0)
    #define VN_ERROR(...)   ((void)0)
    #define VN_ASSERT(...)  ((void)0)

#else
    #define VN_DEBUG(...)   ::vn::Logger::debug(__VA_ARGS__)
    #define VN_INFO(...)    ::vn::Logger::info(__VA_ARGS__)
    #define VN_WARNING(...) ::vn::Logger::warning(__VA_ARGS__)
    #define VN_ERROR(...)   ::vn::Logger::error(__VA_ARGS__)
    #define VN_FATAL(...)   ::vn::Logger::fatal(__FILE__, __LINE__, __VA_ARGS__)
    #define VN_ASSERT(cond, ...)                                                        \
        do {                                                                            \
            if (!(cond)) {                                                              \
                ::vn::Logger::assertion_failed(#cond, __FILE__, __LINE__, __VA_ARGS__); \
            }                                                                           \
        } while (false)

#endif