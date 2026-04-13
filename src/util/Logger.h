#pragma once
#include <iostream>
#include <string>

class Logger {
public:
    enum class Level { INFO, WARN, ERROR };

    static void init(const std::string& path, Level level) {
        // For now, just print to stdout (ignore file/log level)
        (void)path;
        (void)level;
    }

    static void info(const std::string& msg) {
        std::cout << "[INFO] " << msg << std::endl;
    }

    static void warn(const std::string& msg) {
        std::cout << "[WARN] " << msg << std::endl;
    }

    static void error(const std::string& msg) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
};
