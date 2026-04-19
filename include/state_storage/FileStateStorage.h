#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <chrono>

struct StoredFileInfo {
    std::filesystem::file_time_type lastWriteTime;
    uintmax_t size;
};

class FileStateStorage {
public:
    explicit FileStateStorage(const std::string& filename);
    
    void load(std::unordered_map<std::string, StoredFileInfo>& state);
    void save(const std::unordered_map<std::string, StoredFileInfo>& state);
    bool exists() const;
    void clear();

private:
    std::string filename_;
};