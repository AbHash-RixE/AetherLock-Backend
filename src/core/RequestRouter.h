#pragma once
#include "ipc/Protocol.h"
#include "core/Locker.h"
#include "security/Whitelist.h"
#include "util/Logger.h"
#include <memory>
#include <unistd.h>     // for fork(), execvp(), execlp()
#include <sys/types.h>  // for pid_t

class RequestRouter {
    std::shared_ptr<Locker> locker;
    std::shared_ptr<Whitelist> whitelist;

public:
    RequestRouter(std::shared_ptr<Locker> l, std::shared_ptr<Whitelist> w)
        : locker(l), whitelist(w) {}

    Response handle(const Request& req) {
        Response res;
        res.request_id = req.request_id;

        if (req.action == "lock") {
            locker->lock();
            res.status = "ok";
            res.message = "Workspace locked";

        } else if (req.action == "unlock") {
            std::string pass = req.params.value("password", "");
            if (pass.empty()) {
                res.status = "error";
                res.message = "Missing password";
            } else if (locker->unlock(pass)) {
                res.status = "ok";
                res.message = "Unlocked";
            } else {
                res.status = "error";
                res.message = "Invalid password";
            }

        } else if (req.action == "launchApp") {
            std::string app = req.params.value("name", "");
            if (app.empty()) {
                res.status = "error";
                res.message = "Missing app name";
            } else if (whitelist->isAllowed(app)) {
                pid_t pid = fork();
                if (pid==0){
                    execlp(app.c_str(), app.c_str(), nullptr);
                    Logger::info("App launch success");
                }else if (pid>0){
                    res.status = "ok";
                    res.message = "Launched " + app;
                }else {
                    res.status="error";
                    res.message="Failed to fork process";
                }
            } else {
                res.status = "error";
                res.message = "App not allowed: " + app;
            }

        } else {
            res.status = "error";
            res.message = "Unknown action: " + req.action;
        }

        return res;
    }
};
