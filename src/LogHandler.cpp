#include <iostream>
#include "event_dispatcher/EventDispatcher.h"

void LogHandler(const Event& e) {
    std::cout << "[LOG] Событие '" << e.type << "': " << e.payload << std::endl;
}