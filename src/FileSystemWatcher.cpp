#include "system_watcher/FileSystemWatcher.h"
#include <iostream>

FileSystemWatcher::FileSystemWatcher(const std::string& path, EventDispatcher& dispatcher)
    : path_(path), dispatcher_(dispatcher) {}

void FileSystemWatcher::scan() {
    std::unordered_map<std::string, FileInfo> currentState;

    for (const auto& entry : std::filesystem::directory_iterator(path_)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().string();

        FileInfo info{
            entry.last_write_time(),
            entry.file_size()
        };

        currentState[filename] = info;

        // 🆕 Новый файл
        if (previousState_.find(filename) == previousState_.end()) {
            dispatcher_.dispatch({"file_created", filename});
        }
        else {
            // 🔄 Изменён
            auto& old = previousState_[filename];
            if (old.lastWriteTime != info.lastWriteTime) {
                dispatcher_.dispatch({"file_modified", filename});
            }
        }
    }

    // ❌ Удалённые
    for (const auto& [filename, _] : previousState_) {
        if (currentState.find(filename) == currentState.end()) {
            dispatcher_.dispatch({"file_deleted", filename});
        }
    }

    previousState_ = currentState;
}