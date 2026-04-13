#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct Request {
    std::string request_id;
    std::string action;
    nlohmann::json params;
};

struct Response {
    std::string request_id;
    std::string status;   // "ok" or "error"
    nlohmann::json data;
    std::string message;
};

// ---- JSON Converters ----
inline void to_json(nlohmann::json& j, const Request& r) {
    j = { {"request_id", r.request_id}, {"action", r.action}, {"params", r.params} };
}
inline void from_json(const nlohmann::json& j, Request& r) {
    j.at("request_id").get_to(r.request_id);
    j.at("action").get_to(r.action);
    if (j.contains("params")) r.params = j.at("params");
}

inline void to_json(nlohmann::json& j, const Response& r) {
    j = { {"request_id", r.request_id}, {"status", r.status}, {"data", r.data}, {"message", r.message} };
}
inline void from_json(const nlohmann::json& j, Response& r) {
    j.at("request_id").get_to(r.request_id);
    j.at("status").get_to(r.status);
    j.at("data").get_to(r.data);
    j.at("message").get_to(r.message);
}
