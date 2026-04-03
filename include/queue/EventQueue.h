#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "../event_dispatcher/EventDispatcher.h"

struct QueuedEvent {
    Event event;
    int attempts = 0;
    std::chrono::steady_clock::time_point nextRetryTime;
};

class EventQueue {
public:
    void push(const Event& e);
    bool pop(QueuedEvent& outEvent, int timeoutMs = 100);

    void markProcessed(const Event& e);           // ACK
    void markFailed(const Event& e);              // NACK → retry или drop

    size_t size() const;
    bool empty() const;

private:
    std::queue<QueuedEvent> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};