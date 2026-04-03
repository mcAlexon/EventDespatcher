#include <iostream>
#include "event_dispatcher/EventDispatcher.h"

void EchoHandler(const Event& e) {
    std::cout << "[ECHO] Получено: " << e.payload << std::endl;
}