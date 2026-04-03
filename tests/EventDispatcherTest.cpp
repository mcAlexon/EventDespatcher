#include <gtest/gtest.h>
#include "event_dispatcher/EventDispatcher.h"
#include <string>

TEST(EventDispatcherTest, RegistersAndDispatches) {
    EventDispatcher dispatcher;

    std::string logResult, echoResult;

    dispatcher.registerHandler("test", [&](const Event& e) {
        logResult = e.payload;
    });
    dispatcher.registerHandler("test", [&](const Event& e) {
        echoResult = "echo_" + e.payload;
    });

    Event ev{"test", "hello_world"};
    dispatcher.dispatch(ev);

    EXPECT_EQ(logResult, "hello_world");
    EXPECT_EQ(echoResult, "echo_hello_world");
}