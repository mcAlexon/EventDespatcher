#include <gtest/gtest.h>
#include "logger/EventLogger.h"
#include "event_dispatcher/EventDispatcher.h"
#include <chrono>
#include <thread>

TEST(EventSystemLoadTest, HandlesThousandEvents) {
    EventLogger logger("logs/load_test.log");
    EventDispatcher dispatcher;

    int processedCount = 0;

    dispatcher.registerHandler("load_test", [&](const Event& e) {
        logger.logEvent(e, EventStatus::PROCESSED, "Нагрузочный тест");
        processedCount++;
    });

    const int TOTAL_EVENTS = 1000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < TOTAL_EVENTS; ++i) {
        dispatcher.dispatch({"load_test", "event_" + std::to_string(i)});
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_EQ(processedCount, TOTAL_EVENTS);

    std::cout << "✅ Нагрузочное тестирование: " 
              << TOTAL_EVENTS << " событий обработано за " 
              << duration.count() << " мс\n";
}