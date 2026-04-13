#pragma once
#include "Whitelist.h"
#include "util/Logger.h"
#include <thread>
#include <atomic>
#include <chrono>

class Monitor {
    std::shared_ptr<Whitelist> whitelist;
    std::thread worker;
    std::atomic<bool> running{false};

public:
    Monitor(std::shared_ptr<Whitelist> wl) : whitelist(wl) {}

    void start() {
        running = true;
        worker = std::thread([this]() {
            while (running) {
                // Dummy: no real process check yet
                std::this_thread::sleep_for(std::chrono::seconds(2));
                Logger::info("Monitor heartbeat...");
            }
        });
    }

    void stop() {
        running = false;
        if (worker.joinable()) worker.join();
    }
};