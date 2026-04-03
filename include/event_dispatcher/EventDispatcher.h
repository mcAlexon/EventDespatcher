#pragma once

#include <functional>       // ← для std::function
#include <string>           // ← для std::string
#include <unordered_map>    // ← для std::unordered_map
#include <vector>           // ← для std::vector

struct Event {
    std::string type;
    std::string payload;
};

class EventDispatcher {
public:
    using Handler = std::function<void(const Event&)>;

    void registerHandler(const std::string& eventType, Handler handler);
    void dispatch(const Event& event);

private:
    std::unordered_map<std::string, std::vector<Handler>> handlers_;
};