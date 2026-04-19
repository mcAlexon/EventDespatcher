cmake_minimum_required(VERSION 3.14)
project(FS-EventHub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ====================== Пути ======================
include_directories(${PROJECT_SOURCE_DIR}/include)

# ====================== Поиск пакетов ======================
find_package(nlohmann_json QUIET)   # QUIET — чтобы не падал, если не через cmake

# ====================== Основные библиотеки ======================

add_library(event_dispatcher
    src/EventDispatcher.cpp
)

add_library(handlers
    src/LogHandler.cpp
    src/EchoHandler.cpp
)

# ====================== Redis ======================
add_library(redis_client
    src/RedisClient.cpp
)
target_link_libraries(redis_client PRIVATE hiredis)

# ====================== Logger ======================
add_library(event_logger
    src/EventLogger.cpp
)

# ====================== Queue ======================
add_library(event_queue
    src/EventQueue.cpp
)

# ====================== State Storage ======================
add_library(state_storage
    src/FileStateStorage.cpp
)

# ====================== FileSystemWatcher ======================
add_library(system_watcher
    src/FileSystemWatcher.cpp
)

target_link_libraries(system_watcher PRIVATE 
    state_storage 
    event_queue          # ← ЭТО БЫЛО ГЛАВНОЙ ПРИЧИНОЙ!
)

# ====================== ThreadPool (ЛР5) ======================
add_library(thread_pool
    src/ThreadPool.cpp
)

# ====================== Приложение ======================
add_executable(app
    src/main.cpp
    src/RedisPublisherHandler.cpp
)

target_link_libraries(app PRIVATE
    event_dispatcher
    handlers
    redis_client
    event_logger
    event_queue
    system_watcher
    state_storage
    thread_pool
    pthread
)

# Если nlohmann_json найден через find_package
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(state_storage PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json)
endif()

# ====================== Тесты ======================
# ====================== Тесты ======================
enable_testing()
find_package(GTest REQUIRED)

add_executable(tests
    tests/EventLoadTestAdvanced.cpp
)

target_link_libraries(tests PRIVATE
    event_dispatcher
    handlers
    event_logger
    event_queue          # ← обязательно
    system_watcher       # ← обязательно
    state_storage
    thread_pool
    GTest::GTest
    GTest::Main
    pthread
)

include(GoogleTest)
gtest_discover_tests(tests)
# ====================== Пост-действия ======================
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        ${CMAKE_BINARY_DIR}/watched
        ${CMAKE_BINARY_DIR}/logs
        ${CMAKE_BINARY_DIR}/storage
    COMMENT "Создание рабочих директорий"
)