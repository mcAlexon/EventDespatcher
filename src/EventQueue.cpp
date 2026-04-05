#include "queue/EventQueue.h"
#include <iostream>

void EventQueue::push(const Event& e) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push({e, 0, {}});
    cv_.notify_one();
}

bool EventQueue::pop(QueuedEvent& outEvent, int timeoutMs) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (!cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                      [this] { return !queue_.empty(); })) {
        return false;
    }

    outEvent = queue_.front();
    queue_.pop();
    return true;
}

void EventQueue::markProcessed(const Event& e) {
    std::string key = e.type + "|" + e.payload;
    std::cout << "[Queue] ✓ ACK: " << e.type << " | " << e.payload << std::endl;
    attemptCounts_.erase(key);
}

void EventQueue::markFailed(const Event& e) {
    std::string key = e.type + "|" + e.payload;
    int currentAttempts = attemptCounts_[key];  // 0 если новая

    std::cout << "[Queue] ✗ NACK: " << e.type << " | " << e.payload 
              << " (попытка #" << (currentAttempts + 1) << ")" << std::endl;

    if (currentAttempts < 2) {  // 3 попытки всего
        attemptCounts_[key] = currentAttempts + 1;
        push(e);
        std::cout << "[Queue] → Повторная попытка #" << (currentAttempts + 2) << std::endl;
    } else {
        std::cout << "[Queue] ✗ Сброшен после 3 попыток" << std::endl;
        attemptCounts_.erase(key);
    }
}

size_t EventQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

bool EventQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}