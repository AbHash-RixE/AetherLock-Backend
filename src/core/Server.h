#pragma once
#include "core/RequestRouter.h"
#include "util/Logger.h"
#include "ipc/Protocol.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <cstring>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>


class Server {
    std::string socketPath;
    std::shared_ptr<RequestRouter> router;
    std::thread worker;
    std::atomic<bool> running{false};
    int server_fd{-1};

public:
    Server(const std::string& path, std::shared_ptr<RequestRouter> r)
        : socketPath(path), router(r) {}

    void start() {
        running = true;
        worker = std::thread([this]() { run(); });
    }

    void stop() {
        running = false;
        if (server_fd >= 0){
            shutdown(server_fd, SHUT_RDWR);
        }
        if (worker.joinable()) worker.join();
        if (!socketPath.empty()) unlink(socketPath.c_str());
    }

private:
    void run() {
        server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_fd < 0) {
            Logger::error("Failed to create socket");
            return;
        }

        struct sockaddr_un addr{};
        unlink(socketPath.c_str());
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path)-1);

        if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            Logger::error("Failed to bind socket");
            close(server_fd);
            return;
        }

        if (listen(server_fd, 5) < 0) {
            Logger::error("Failed to listen on socket");
            close(server_fd);
            return;
        }

        Logger::info("Server listening on " + socketPath);

        while (running) {
            int client_fd = accept(server_fd, nullptr, nullptr);
            if (client_fd < 0){
                if (!running) break;
                Logger::error("Accept failed");
                continue;
            }

            char buf[2048];
            int n = read(client_fd, buf, sizeof(buf)-1);
            if (n > 0) {
                buf[n] = '\0';

                try {
                    nlohmann::json j = nlohmann::json::parse(buf);
                    Request req = j.get<Request>();
                    Response res = router->handle(req);

                    std::string responseStr = nlohmann::json(res).dump();
                    write(client_fd, responseStr.c_str(), responseStr.size());
                } catch (const std::exception& e) {
                    Logger::error(std::string("Invalid request: ") + e.what());
                    Response err;
                    err.request_id="0";
                    err.status="error";
                    err.message=e.what();
                    std::string errStr=nlohmann::json(err).dump();
                    write(client_fd, errStr.c_str(), errStr.size());
                }
            }
            close(client_fd);
        }
        close(server_fd);
        server_fd=-1;
        Logger::info("Server stopped");
    }
};