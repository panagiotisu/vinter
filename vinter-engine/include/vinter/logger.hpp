#pragma once

#include <iostream>

namespace vn {
    class Logger {
    public:
        enum class Level {
            Debug,
            Info,
            Warning,
            Error
        };

        Logger() = default;

        static void debug(std::string_view msg);
        static void info(std::string_view msg);
        static void warning(std::string_view msg);
        static void error(std::string_view msg);

    private:
        static void log(Level level, std::string_view message);
        static std::string_view to_string(Level level);
    };
} // vn