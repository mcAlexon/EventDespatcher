#include "watcher/FileWatcher.h"
#include <iostream>
#include <cstring>

FileWatcher::FileWatcher(EventQueue& queue, const std::string& watchPath)
    : queue_(queue), watchPath_(watchPath) {}

FileWatcher::~FileWatcher() {
    stop();
}

void FileWatcher::start() {
    if (running_) return;
    running_ = true;
    watcherThread_ = std::thread(&FileWatcher::watchLoop, this);
}

void FileWatcher::stop() {
    running_ = false;
    if (watcherThread_.joinable()) watcherThread_.join();
}

void FileWatcher::watchLoop() {
    inotifyFd_ = inotify_init();
    if (inotifyFd_ < 0) {
        std::cerr << "[FileWatcher] inotify_init failed" << std::endl;
        return;
    }

    watchDescriptor_ = inotify_add_watch(inotifyFd_, watchPath_.c_str(),
                                         IN_CREATE | IN_MODIFY | IN_DELETE);

    if (watchDescriptor_ < 0) {
        std::cerr << "[FileWatcher] Cannot watch directory: " << watchPath_ << std::endl;
        close(inotifyFd_);
        return;
    }

    std::cout << "[FileWatcher] Наблюдение за папкой " << watchPath_ << " запущено\n";

    char buffer[4096];
    while (running_) {
        int length = read(inotifyFd_, buffer, sizeof(buffer));
        if (length < 0) break;

        int i = 0;
        while (i < length) {
            inotify_event* event = (inotify_event*)&buffer[i];

            std::string filename = event->name;
            std::string fullPath = watchPath_ + "/" + filename;

            Event e;
            if (event->mask & IN_CREATE) {
                e = {"file_created", fullPath};
            } else if (event->mask & IN_MODIFY) {
                e = {"file_modified", fullPath};
            } else if (event->mask & IN_DELETE) {
                e = {"file_deleted", fullPath};
            }

            if (!e.type.empty()) {
                queue_.push(e);
            }

            i += sizeof(inotify_event) + event->len;
        }
    }

    close(inotifyFd_);
}