#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include "event_dispatcher/EventDispatcher.h"
#include "../state_storage/FileStateStorage.h"   // ← добавлено

struct FileInfo {
    std::filesystem::file_time_type lastWriteTime;
    uintmax_t size;
};

class FileSystemWatcher {
public:
    FileSystemWatcher(const std::string& path, EventDispatcher& dispatcher);

    void scan(); // один проход

private:
    std::string path_;
    EventDispatcher& dispatcher_;
    std::unordered_map<std::string, FileInfo> previousState_;
    FileStateStorage storage_;   // ← добавлено для JSON-персистентности

    void loadState();
    void saveState();
};