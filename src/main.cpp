#include "core/Server.h"
#include "core/RequestRouter.h"
#include "core/Locker.h"
#include "security/Whitelist.h"
#include "security/Monitor.h"
#include "util/Logger.h"
#include <memory>
#include <csignal>

volatile std::sig_atomic_t g_running=1;

// Signal handler for SIGINT (Ctrl+C) or SIGTERM
void signalHandler(int signum){
    g_running=0;
    Logger::info("Shutting down backend daemon...");
}

int main(){
    Logger::init("/var/log/backend.log" , Logger::Level::INFO);
    Logger::info("Backend daemon starting...");

    // Create Locker (tracks lock/unlock state)
    auto locker = std::make_shared<Locker>();

    // Create Whitelist (allowed apps)
    auto whitelist = std::make_shared<Whitelist>();
    whitelist->loadFromConfig("/etc/backend/allowed_apps.json"); // optional

    // Create Monitor (optional background enforcement)
    auto monitor = std::make_shared<Monitor>(whitelist);
    monitor->start(); // runs in background thread

    auto router = std::make_shared<RequestRouter>(locker, whitelist);

    Server server("/tmp/backend.sock", router);
    server.start(); // runs in its own thread or blocking

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cleanup
    monitor->stop();
    server.stop();
    Logger::info("Backend daemon exited cleanly");
    return 0;

}