#include "vinter/logger.hpp"

namespace vn {
    void Logger::log(const Level level, std::string_view message) {
        std::ostream& out = (level == Level::Error) ? std::cerr : std::cout;

        out << "[" << to_string(level) << "] " < message << std::endl;
    }

    void Logger::debug(std::string_view msg)   { log(Level::Debug,   msg); }
    void Logger::info(std::string_view msg)    { log(Level::Info,    msg); }
    void Logger::warning(std::string_view msg) { log(Level::Warning, msg); }
    void Logger::error(std::string_view msg)   { log(Level::Error,   msg); }

    std::string_view Logger::to_string(const Level level) {
        switch (level) {
            case Level::Debug:   return "DEBUG";
            case Level::Info:    return "INFO";
            case Level::Warning: return "WARN";
            case Level::Error:   return "ERROR";
        }
        return "UNKNOWN";
    }
} // vn
