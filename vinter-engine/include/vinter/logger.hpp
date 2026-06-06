#pragma once

#include <cstdint>
#include <format>
#include <iostream>

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

        static void enable_colors(bool enabled) {
            m_use_colors = enabled;
        }

        template <typename... Args>
        static void debug(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Debug, "\033[90m", "[DEBUG]", fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void info(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Info, "\033[32m", "[INFO]", fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void warning(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Warning, "\033[33m", "[WARNING]", fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void error(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Error, "\033[31m", "[ERROR]", fmt, std::forward<Args>(args)...);
        }

    private:
        template <typename... Args>
        static void
        log(Level level,
            const char* color,
            const char* label,
            std::format_string<Args...> fmt,
            Args&&... args) {
            if (level < m_level_filter) {
                return;
            }

            const auto message = std::format(fmt, std::forward<Args>(args)...);

            if (m_use_colors) {
                std::cout << color << label << ": " << message << "\033[0m"
                          << "\n";
            } else {
                std::cout << label << ": " << message << "\n";
            }
        }

        inline static Level m_level_filter {Level::Info};
        inline static bool m_use_colors {true};
    };

} // namespace vn