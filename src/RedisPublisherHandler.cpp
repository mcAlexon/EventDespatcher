#include <iostream>
#include "redis/RedisClient.h"
#include "event_dispatcher/EventDispatcher.h"

// Глобальный клиент Redis (для простоты на данном этапе)
extern RedisClient redisClient;

void RedisPublisherHandler(const Event& e) {
    std::cout << "[RedisPublisher] Публикуем событие: " << e.type 
              << " | " << e.payload << std::endl;

    if (redisClient.publish("fs_events", e)) {
        std::cout << "[RedisPublisher] ✓ Успешно отправлено в Redis\n" << std::endl;
    } else {
        std::cerr << "[RedisPublisher] ✗ Ошибка отправки в Redis\n" << std::endl;
    }
}