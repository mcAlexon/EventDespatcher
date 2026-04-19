#include <gtest/gtest.h>
#include "queue/EventQueue.h"
#include "thread_pool/ThreadPool.h"
#include "system_watcher/FileSystemWatcher.h"
#include "event_dispatcher/EventDispatcher.h"

#include <filesystem>
#include <thread>
#include <vector>
#include <fstream>        // ← ОБЯЗАТЕЛЬНО для std::ofstream
#include <chrono>
#include <iostream>

TEST(EventSystemLoadTest, ConcurrentCreateModifyDelete) {
    EventQueue queue;
    ThreadPool pool(8);
    EventDispatcher dispatcher;

    // Создаём временную папку для теста
    const std::string testDir = "test_load_dir";
    std::filesystem::create_directories(testDir);

    FileSystemWatcher watcher(testDir, dispatcher, queue);

    int processed = 0;

    dispatcher.registerHandler("file_created",  [&](const Event&) { processed++; });
    dispatcher.registerHandler("file_modified", [&](const Event&) { processed++; });
    dispatcher.registerHandler("file_deleted",  [&](const Event&) { processed++; });

    const int FILES = 150;        // можно увеличить
    std::vector<std::thread> workers;

    auto start = std::chrono::high_resolution_clock::now();

    // 8 параллельных потоков имитируют нагрузку
    for (int i = 0; i < 8; ++i) {
        workers.emplace_back([&, i]() {
            for (int j = 0; j < FILES / 8; ++j) {
                std::string name = testDir + "/file_" + std::to_string(i) + "_" + std::to_string(j) + ".txt";

                // Create
                std::ofstream(name).close();
                std::this_thread::sleep_for(std::chrono::milliseconds(3));

                // Modify
                std::ofstream(name, std::ios::app) << "some data " << j;
                std::this_thread::sleep_for(std::chrono::milliseconds(3));

                // Delete
                std::filesystem::remove(name);
            }
        });
    }

    for (auto& t : workers) t.join();

    // Даём время очереди и ThreadPool обработать все события
    std::this_thread::sleep_for(std::chrono::seconds(4));

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\n=== Нагрузочный тест завершён ===\n";
    std::cout << "Создано/изменено/удалено файлов: " << FILES << "\n";
    std::cout << "Время выполнения: " << duration.count() << " мс\n";
    std::cout << "Обработано событий: " << processed << "\n";
    std::cout << "Скорость: " << (processed * 1000.0 / duration.count()) << " событий/сек\n";

    EXPECT_GT(processed, FILES * 2);  // минимум create + delete на каждый файл

    // Очистка
    std::filesystem::remove_all(testDir);
}