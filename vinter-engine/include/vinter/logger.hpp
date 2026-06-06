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
            Fatal,
        };

        static void set_level_filter(Level level) {
            m_level_filter = level;
        }

        template <typename... Args>
        static void debug(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Debug, "[DEBUG]", fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void info(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Info, "[INFO]", fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void warning(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Warning, "[WARNING]", fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void error(std::format_string<Args...> fmt, Args&&... args) {
            log(Level::Error, "[ERROR]", fmt, std::forward<Args>(args)...);
        }

    private:
        template <typename... Args>
        static void
        log(Level level, const char* prefix, std::format_string<Args...> fmt, Args&&... args) {
            if (level < m_level_filter) {
                return;
            }
            std::cout << prefix << std::format(fmt, std::forward<Args>(args)...) << "\n";
        }

        inline static Level m_level_filter {Level::Info};
    };
} // namespace vn