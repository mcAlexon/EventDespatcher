#include "queue/EventQueue.h"
#include <iostream>
#include <thread>

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
    std::cout << "[Queue] ✓ ACK: " << e.type << " | " << e.payload << std::endl;
}

void EventQueue::markFailed(const Event& e) {
    std::cout << "[Queue] ✗ NACK: " << e.type << " | " << e.payload << std::endl;
}