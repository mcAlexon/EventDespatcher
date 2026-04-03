#include "queue/EventQueue.h"
#include <iostream>

void EventQueue::push(const Event& e) {
    std::lock_guard<std::mutex> lock(mutex_);
    QueuedEvent qe{ e, 0, {} };
    queue_.push(qe);
    cv_.notify_one();
}

bool EventQueue::pop(QueuedEvent& outEvent, int timeoutMs) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto timeout = std::chrono::milliseconds(timeoutMs);
    if (!cv_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
        return false; // таймаут
    }

    outEvent = queue_.front();
    queue_.pop();
    return true;
}

void EventQueue::markProcessed(const Event& e) {
    // Для логов (можно расширить позже)
    std::cout << "[Queue] ✓ ACK: " << e.type << " | " << e.payload << std::endl;
}

void EventQueue::markFailed(const Event& e) {
    std::cout << "[Queue] ✗ NACK: " << e.type << " | " << e.payload << std::endl;
}

size_t EventQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

bool EventQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}