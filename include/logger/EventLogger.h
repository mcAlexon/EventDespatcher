#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include "../event_dispatcher/EventDispatcher.h"

enum class EventStatus {
    RECEIVED, PROCESSED, FAILED, RETRY, DROPPED
};

class EventLogger {
public:
    explicit EventLogger(const std::string& logFilePath = "logs/events.log");
    ~EventLogger();

    void logEvent(const Event& event, EventStatus status, const std::string& message = "");
    void logSeparator();                    // ← новая функция

private:
    std::ofstream logFile_;
    std::mutex mutex_;
    std::string logFilePath_;

    std::string statusToString(EventStatus status) const;
    std::string currentTimestamp() const;
};