#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <hiredis/hiredis.h>
#include "../event_dispatcher/EventDispatcher.h"

class RedisClient {
public:
    explicit RedisClient(const std::string& host = "host.docker.internal", int port = 6379);
    ~RedisClient();

    bool connect();
    void disconnect();

    bool publish(const std::string& channel, const Event& event);

    void startSubscriber(const std::string& channel, 
                         std::function<void(const Event&)> onEventReceived);

    void stopSubscriber();

private:
    std::string host_;
    int port_;
    redisContext* context_ = nullptr;

    std::thread subscriberThread_;
    std::atomic<bool> running_{false};

    static std::string eventToString(const Event& e);
    static Event stringToEvent(const std::string& str);
};