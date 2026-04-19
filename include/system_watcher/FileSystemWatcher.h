#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include "event_dispatcher/EventDispatcher.h"
#include "../state_storage/FileStateStorage.h"
#include "../queue/EventQueue.h"     // ← ОБЯЗАТЕЛЬНО

struct FileInfo {
    std::filesystem::file_time_type lastWriteTime;
    uintmax_t size;
};

class FileSystemWatcher {
public:
    // Новый конструктор с очередью
    FileSystemWatcher(const std::string& path, EventDispatcher& dispatcher, EventQueue& queue);

    void scan(); 

private:
    std::string path_;
    EventDispatcher& dispatcher_;
    EventQueue& queue_;                    // ← добавлено
    std::unordered_map<std::string, FileInfo> previousState_;
    FileStateStorage storage_;

    void loadState();
    void saveState();
};