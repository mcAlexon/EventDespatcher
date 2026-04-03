#include <iostream>
#include <chrono>
#include <thread>
#include "event_dispatcher/EventDispatcher.h"
#include "redis/RedisClient.h"

// Глобальный Redis клиент
RedisClient redisClient("host.docker.internal", 6379);

void LogHandler(const Event& e) {
    std::cout << "[LOG] " << e.type << ": " << e.payload << std::endl;
}

void EchoHandler(const Event& e) {
    std::cout << "[ECHO] → " << e.payload << std::endl;
}

void RedisPublisherHandler(const Event& e);   // объявление

int main() {
    EventDispatcher dispatcher;

    std::cout << "=== FS-EventHub v0.3 (ЛР3) ===\n\n";

    // Подключаемся к Redis
    if (!redisClient.connect()) {
        std::cerr << "Не удалось подключиться к Redis!\n";
        return 1;
    }

    // Запускаем подписчика на Redis (в отдельном потоке)
    redisClient.startSubscriber("fs_events", [&](const Event& e) {
        std::cout << "[RedisListener] Получено из Redis → " 
                  << e.type << " | " << e.payload << std::endl;
    });

    // Регистрируем обработчики
    dispatcher.registerHandler("file_created", LogHandler);
    dispatcher.registerHandler("file_created", EchoHandler);
    dispatcher.registerHandler("file_created", RedisPublisherHandler);

    std::cout << "Система запущена. Ожидаем событий...\n";
    std::cout << "Нажмите Enter для отправки тестового события...\n\n";

    // Тестовое событие
    dispatcher.dispatch({"file_created", "/watched/document.pdf"});

    std::cin.get(); // Ждём нажатия Enter

    std::cout << "\nЗавершение работы...\n";
    redisClient.stopSubscriber();

    return 0;
}