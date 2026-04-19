#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = 4);
    ~ThreadPool();

    void submit(std::function<void()> task);

    size_t activeThreads() const;
    size_t queuedTasks() const;
    size_t totalProcessed() const;

    void shutdown();

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{false};
    std::atomic<size_t> activeCount_{0};
    std::atomic<size_t> processedCount_{0};
};