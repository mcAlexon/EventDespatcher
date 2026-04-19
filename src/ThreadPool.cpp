#include "thread_pool/ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

                    if (stop_ && tasks_.empty()) return;

                    task = std::move(tasks_.front());
                    tasks_.pop();
                    activeCount_++;
                }

                task();
                processedCount_++;
                activeCount_--;
            }
        });
    }
    std::cout << "[ThreadPool] Создан пул из " << numThreads << " потоков\n";
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::submit(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stop_) return;
        tasks_.push(std::move(task));
    }
    cv_.notify_one();
}

void ThreadPool::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }
    cv_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) worker.join();
    }
}

size_t ThreadPool::activeThreads() const { return activeCount_; }
size_t ThreadPool::queuedTasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tasks_.size();
}
size_t ThreadPool::totalProcessed() const { return processedCount_; }