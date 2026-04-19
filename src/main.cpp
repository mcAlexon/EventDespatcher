#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include "event_dispatcher/EventDispatcher.h"
#include "redis/RedisClient.h"
#include "logger/EventLogger.h"
#include "queue/EventQueue.h"
#include "system_watcher/FileSystemWatcher.h"
#include "thread_pool/ThreadPool.h"

RedisClient redisClient("host.docker.internal", 6379);
EventLogger eventLogger("logs/events.log");
EventQueue eventQueue;
ThreadPool threadPool(4);

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
    std::filesystem::create_directories("watched");
    std::filesystem::create_directories("logs");
    std::filesystem::create_directories("storage");

    EventDispatcher dispatcher;

    std::cout << "=== FS-EventHub v0.5 (ЛР5 — Полная асинхронная обработка) ===\n\n";

    if (!redisClient.connect()) return 1;

    redisClient.startSubscriber("fs_events", [&](const Event& e) {
        std::cout << "[RedisListener] Получено из Redis: " << e.type << " | " << e.payload << std::endl;
        eventLogger.logEvent(e, EventStatus::RECEIVED, "Из Redis");
    });

    // Регистрация обработчиков
    dispatcher.registerHandler("file_created",  LogHandler);
    dispatcher.registerHandler("file_created",  EchoHandler);
    dispatcher.registerHandler("file_created",  RedisPublisherHandler);
    dispatcher.registerHandler("file_modified", LogHandler);
    dispatcher.registerHandler("file_modified", RedisPublisherHandler);
    dispatcher.registerHandler("file_deleted",  LogHandler);
    dispatcher.registerHandler("file_deleted",  RedisPublisherHandler);

    // Watcher теперь пушит в очередь
    FileSystemWatcher watcher("watched", dispatcher, eventQueue);

    // Consumer → ThreadPool
    std::thread consumerThread([&]() {
        std::cout << "[Consumer] Запущен → события идут в ThreadPool\n";
        while (true) {
            QueuedEvent qe;
            if (eventQueue.pop(qe, 300)) {
                threadPool.submit([qe, &dispatcher]() mutable {
                    try {
                        dispatcher.dispatch(qe.event);
                        eventQueue.markProcessed(qe.event);
                    } catch (...) {
                        eventQueue.markFailed(qe.event);
                    }
                });
            }
        }
    });

    // Метрики
    std::thread metricsThread([&]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "[Metrics] Активных: " << threadPool.activeThreads()
                      << " | В очереди: " << threadPool.queuedTasks()
                      << " | Всего обработано: " << threadPool.totalProcessed() << "\n";
        }
    });

    std::cout << "✅ Система работает через EventQueue + ThreadPool (4 потока)\n\n";

    while (true) {
        watcher.scan();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    consumerThread.join();
    metricsThread.join();
    threadPool.shutdown();
    redisClient.stopSubscriber();
    return 0;
}