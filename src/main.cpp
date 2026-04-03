// src/main.cpp
#include <iostream>
#include "event_dispatcher/EventDispatcher.h"

void LogHandler(const Event& e) {
    std::cout << "[LOG] " << e.type << ": " << e.payload << "\n";
}

void EchoHandler(const Event& e) {
    std::cout << "[ECHO] Получено → " << e.payload << "\n";
}

int main() {
    EventDispatcher disp;

    // Регистрируем обработчики
    disp.registerHandler("user_login", LogHandler);
    disp.registerHandler("user_login", EchoHandler);
    disp.registerHandler("message", LogHandler);

    std::cout << "=== Тестируем диспетчер ===\n\n";

    disp.dispatch({"user_login", "alice вошла в систему"});
    disp.dispatch({"message", "Привет, как дела?"});
    disp.dispatch({"unknown", "это никто не обработает"});

    std::cout << "\nГотово.\n";
    return 0;
}