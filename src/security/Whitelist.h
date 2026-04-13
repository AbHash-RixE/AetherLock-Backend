#pragma once

#include <set>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "util/Logger.h"

class Whitelist {
    std::set<std::string> allowedApps;

public:
    void loadFromConfig(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            Logger::error("Whitelist config not found. Switching to default apps.");
            allowedApps = {"firefox", "code"};
            return;
        }

        try {
            nlohmann::json j;
            file >> j;
            allowedApps.clear();
            for (const auto& app : j["apps"]) {
                allowedApps.insert(app.get<std::string>());
            }
            Logger::info("Whitelist loaded: " + std::to_string(allowedApps.size()) + " apps.");
        } catch (const std::exception& e) {
            Logger::error("Failed to parse whitelist config: " + std::string(e.what()) + ". Switching to default apps.");
            allowedApps = {"firefox", "code"};
        }
    }

    bool isAllowed(const std::string& app) {
        return allowedApps.count(app) > 0;
    }
};
