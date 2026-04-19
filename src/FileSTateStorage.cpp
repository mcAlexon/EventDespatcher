#include "state_storage/FileStateStorage.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

FileStateStorage::FileStateStorage(const std::string& filename)
    : filename_(filename) {
    std::filesystem::path path(filename_);
    auto parent = path.parent_path();
    if (!parent.empty() && !std::filesystem::exists(parent)) {
        std::filesystem::create_directories(parent);
    }
}

void FileStateStorage::load(std::unordered_map<std::string, StoredFileInfo>& state) {
    if (!exists()) {
        std::cout << "[Storage] Файл состояния не найден, начинаем с чистого листа.\n";
        return;
    }

    std::ifstream file(filename_);
    if (!file.is_open()) return;

    try {
        json j;
        file >> j;
        state.clear();

        for (const auto& [path, data] : j.items()) {
            StoredFileInfo info;
            info.size = data["size"];
            
            // Восстанавливаем время
            if (data.contains("last_write_time")) {
                std::string timeStr = data["last_write_time"];
                std::tm tm = {};
                std::istringstream ss(timeStr);
                ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                // Для простоты пока оставляем default time (можно улучшить позже)
            }
            state[path] = info;
        }
        std::cout << "[Storage] Загружено " << state.size() << " файлов из JSON\n";
    } catch (...) {
        std::cerr << "[Storage] Ошибка чтения JSON\n";
    }
}

void FileStateStorage::save(const std::unordered_map<std::string, StoredFileInfo>& state) {
    json j;

    for (const auto& [path, info] : state) {
        j[path] = {
            {"size", info.size},
            {"last_write_time", "2026-04-19 15:00:00"}  // TODO: сделать нормальное время позже
        };
    }

    std::ofstream file(filename_);
    if (file.is_open()) {
        file << j.dump(4);
        std::cout << "[Storage] Сохранено " << state.size() << " записей в JSON\n";
    }
}

bool FileStateStorage::exists() const {
    return std::filesystem::exists(filename_);
}

void FileStateStorage::clear() {
    if (exists()) std::filesystem::remove(filename_);
}