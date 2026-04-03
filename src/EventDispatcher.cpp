#include "event_dispatcher/EventDispatcher.h"

// ← Вот эти две строки были пропущены — они критически важны
#include <string>           // для std::string
#include <unordered_map>    // для std::unordered_map

void EventDispatcher::registerHandler(const std::string& eventType, Handler handler) {
    handlers_[eventType].push_back(handler);
}

void EventDispatcher::dispatch(const Event& event) {
    auto it = handlers_.find(event.type);
    if (it != handlers_.end()) {
        for (const auto& handler : it->second) {
            handler(event);
        }
    }
}