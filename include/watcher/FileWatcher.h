#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <sys/inotify.h>
#include <unistd.h>
#include "../event_dispatcher/EventDispatcher.h"
#include "../queue/EventQueue.h"

class FileWatcher {
public:
    explicit FileWatcher(EventQueue& queue, const std::string& watchPath = "watched");
    ~FileWatcher();

    void start();
    void stop();

private:
    void watchLoop();

    EventQueue& queue_;
    std::string watchPath_;
    int inotifyFd_ = -1;
    int watchDescriptor_ = -1;
    std::thread watcherThread_;
    std::atomic<bool> running_{false};
};