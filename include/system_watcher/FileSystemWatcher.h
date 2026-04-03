#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include "event_dispatcher/EventDispatcher.h"

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

    void loadState();
    void saveState();
};