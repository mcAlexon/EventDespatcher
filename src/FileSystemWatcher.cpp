#include "system_watcher/FileSystemWatcher.h"
#include <iostream>

FileSystemWatcher::FileSystemWatcher(const std::string& path, EventDispatcher& dispatcher)
    : path_(path), dispatcher_(dispatcher), storage_("storage/files.json") {
    loadState();
}

void FileSystemWatcher::loadState() {
    std::unordered_map<std::string, StoredFileInfo> tempState;
    storage_.load(tempState);

    previousState_.clear();
    for (const auto& [path, sInfo] : tempState) {
        FileInfo fInfo;
        fInfo.size = sInfo.size;
        fInfo.lastWriteTime = std::filesystem::file_time_type{};  // default → используем size для offline-проверки
        previousState_[path] = fInfo;
    }
    std::cout << "[Watcher] Загружено состояние " << previousState_.size() 
              << " файлов из storage/files.json\n";
}

void FileSystemWatcher::saveState() {
    std::unordered_map<std::string, StoredFileInfo> tempState;
    for (const auto& [path, fInfo] : previousState_) {
        StoredFileInfo sInfo;
        sInfo.size = fInfo.size;
        sInfo.lastWriteTime = fInfo.lastWriteTime;
        tempState[path] = sInfo;
    }
    storage_.save(tempState);
    std::cout << "[Watcher] Сохранено состояние " << previousState_.size() 
              << " файлов в storage/files.json\n";
}

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
            // 🔄 Изменён (size — для offline-изменений, time — для runtime)
            auto& old = previousState_[filename];
            if (old.size != info.size || 
                (old.lastWriteTime != info.lastWriteTime && old.lastWriteTime != std::filesystem::file_time_type{})) {
                dispatcher_.dispatch({"file_modified", filename});
            }
        }
    }

    // ❌ Удалённые → состояние тоже удаляется из JSON
    for (const auto& [filename, _] : previousState_) {
        if (currentState.find(filename) == currentState.end()) {
            dispatcher_.dispatch({"file_deleted", filename});
        }
    }

    previousState_ = currentState;
    saveState();   // ← сохраняем после каждого скана
}