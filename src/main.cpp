#include <iostream>
#include <filesystem>
#include "event_dispatcher/EventDispatcher.h"
#include "redis/RedisClient.h"
#include "logger/EventLogger.h"

RedisClient redisClient("host.docker.internal", 6379);
EventLogger eventLogger("logs/events.log");

void LogHandler(const Event& e) {
    std::cout << "[LOG] " << e.type << ": " << e.payload << std::endl;
    eventLogger.logEvent(e, EventStatus::PROCESSED, "LogHandler");
}

void EchoHandler(const Event& e) {
    std::cout << "[ECHO] → " << e.payload << std::endl;
    eventLogger.logEvent(e, EventStatus::PROCESSED, "EchoHandler");
}

void RedisPublisherHandler(const Event& e);   // объявление

int main() {
    // Создаём папку watched/ автоматически
    std::filesystem::create_directories("watched");

    EventDispatcher dispatcher;

    std::cout << "=== FS-EventHub v0.4 (ЛР2 + ЛР3 + Logger) ===\n";
    std::cout << "Папка watched/ создана. Создавайте в ней файлы — скоро будем отслеживать в реальном времени.\n\n";

    if (!redisClient.connect()) {
        std::cerr << "Redis не подключился!\n";
        return 1;
    }

    redisClient.startSubscriber("fs_events", [&](const Event& e) {
        std::cout << "[RedisListener] Получено из Redis: " << e.type << " | " << e.payload << std::endl;
        eventLogger.logEvent(e, EventStatus::RECEIVED, "Из Redis");
    });

    dispatcher.registerHandler("file_created", LogHandler);
    dispatcher.registerHandler("file_created", EchoHandler);
    dispatcher.registerHandler("file_created", RedisPublisherHandler);

    std::cout << "Система готова. Логи → logs/events.log\n\n";

    // Тестовые события
    for (int i = 1; i <= 3; ++i) {
        Event e{"file_created", "watched/document_" + std::to_string(i) + ".pdf"};
        dispatcher.dispatch(e);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }

    std::cout << "\nНажмите Enter для завершения...\n";
    std::cin.get();

    redisClient.stopSubscriber();
    std::cout << "Программа завершена.\n";
    return 0;
}