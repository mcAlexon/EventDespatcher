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

# ====================== Основное приложение ======================
add_executable(app
    src/main.cpp
    src/RedisPublisherHandler.cpp
)

target_link_libraries(app PRIVATE
    event_dispatcher
    handlers
    redis_client
    pthread
)

# ====================== Тесты ======================
enable_testing()
find_package(GTest REQUIRED)

add_executable(tests
    tests/EventDispatcherTest.cpp
)

target_link_libraries(tests PRIVATE
    event_dispatcher
    handlers
    GTest::GTest
    GTest::Main
    pthread
)

include(GoogleTest)
gtest_discover_tests(tests)