cmake_minimum_required(VERSION 3.14)
project(FS-EventHub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/include)

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
# ====================== FileSystemWatcher ======================
add_library(system_watcher
    src/FileSystemWatcher.cpp
)
# ====================== Обновлённое приложение ======================
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
    pthread
    system_watcher
)

# ====================== Тесты ======================
enable_testing()
find_package(GTest REQUIRED)

add_executable(tests
    tests/EventDispatcherTest.cpp
    tests/EventLoadTest.cpp        
)

target_link_libraries(tests PRIVATE
    event_dispatcher
    handlers
    event_logger
    GTest::GTest
    GTest::Main
    pthread
)

include(GoogleTest)
gtest_discover_tests(tests)