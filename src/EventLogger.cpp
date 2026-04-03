#include "logger/EventLogger.h"
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>

EventLogger::EventLogger(const std::string& logFilePath) : logFilePath_(logFilePath) {
    std::filesystem::create_directories(std::filesystem::path(logFilePath).parent_path());
    
    logFile_.open(logFilePath_, std::ios::app);
    if (!logFile_.is_open()) {
        std::cerr << "[EventLogger] Не удалось открыть файл лога: " << logFilePath_ << std::endl;
    } else {
        logSeparator();
        logEvent({"SYSTEM", "Logger initialized"}, EventStatus::RECEIVED, "Логгер успешно запущен");
    }
}

EventLogger::~EventLogger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

void EventLogger::logEvent(const Event& event, EventStatus status, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!logFile_.is_open()) return;

    logFile_ << currentTimestamp()
             << " | " << statusToString(status)
             << " | " << event.type
             << " | " << event.payload
             << " | " << message
             << std::endl;
    logFile_.flush();
}

void EventLogger::logSeparator() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!logFile_.is_open()) return;

    logFile_ << "\n" << std::string(90, '=') << "\n"
             << "                    НОВЫЙ ЗАПУСК ПРОГРАММЫ — " << currentTimestamp() << "\n"
             << std::string(90, '=') << "\n\n";
    logFile_.flush();
}

std::string EventLogger::statusToString(EventStatus status) const {
    switch (status) {
        case EventStatus::RECEIVED:  return "RECEIVED";
        case EventStatus::PROCESSED: return "PROCESSED";
        case EventStatus::FAILED:    return "FAILED";
        case EventStatus::RETRY:     return "RETRY";
        case EventStatus::DROPPED:   return "DROPPED";
        default:                     return "UNKNOWN";
    }
}

std::string EventLogger::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now{};
    localtime_r(&time_t_now, &tm_now);   // thread-safe версия

    std::ostringstream ss;
    ss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}