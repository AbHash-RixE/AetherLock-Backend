#pragma once
#include <string>
#include "util/Logger.h"

class Locker {
    bool locked = false;
    std::string password = "secret"; // demo

public:
    void lock() {
        locked = true;
        Logger::info("Workspace locked");
    }

    bool unlock(const std::string& pass) {
        if (pass == password) {
            locked = false;
            Logger::info("Workspace unlocked");
            return true;
        }
        Logger::warn("Failed unlock attempt");
        return false;
    }

    bool isLocked() const { return locked; }
};