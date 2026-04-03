#include "redis/RedisClient.h"
#include <iostream>
#include <sstream>

RedisClient::RedisClient(const std::string& host, int port)
    : host_(host), port_(port) {}

RedisClient::~RedisClient() {
    stopSubscriber();
    disconnect();
}

bool RedisClient::connect() {
    if (context_) return true;

    context_ = redisConnect(host_.c_str(), port_);
    if (!context_ || context_->err) {
        std::cerr << "[Redis] Connection error: " 
                  << (context_ ? context_->errstr : "Can't allocate") << std::endl;
        if (context_) redisFree(context_);
        context_ = nullptr;
        return false;
    }
    std::cout << "[Redis] Успешно подключено к " << host_ << ":" << port_ << std::endl;
    return true;
}

void RedisClient::disconnect() {
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
}

bool RedisClient::publish(const std::string& channel, const Event& event) {
    if (!context_ && !connect()) return false;

    std::string msg = eventToString(event);
    redisReply* reply = (redisReply*)redisCommand(context_, "PUBLISH %s %s", 
                                                  channel.c_str(), msg.c_str());
    
    bool success = (reply != nullptr);
    if (reply) freeReplyObject(reply);
    return success;
}

void RedisClient::startSubscriber(const std::string& channel, 
                                  std::function<void(const Event&)> onEventReceived) {
    if (running_) return;

    running_ = true;
    subscriberThread_ = std::thread([this, channel, onEventReceived]() {
        redisContext* subCtx = redisConnect(host_.c_str(), port_);
        if (!subCtx || subCtx->err) {
            std::cerr << "[Redis Subscriber] Connection failed" << std::endl;
            return;
        }

        redisReply* reply = (redisReply*)redisCommand(subCtx, "SUBSCRIBE %s", channel.c_str());
        freeReplyObject(reply);

        std::cout << "[Redis] Подписка на канал: " << channel << std::endl;

        while (running_ && redisGetReply(subCtx, (void**)&reply) == REDIS_OK) {
            if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements == 3) {
                std::string message = reply->element[2]->str;
                Event e = stringToEvent(message);
                onEventReceived(e);
            }
            if (reply) freeReplyObject(reply);
        }
        redisFree(subCtx);
    });
}

void RedisClient::stopSubscriber() {
    running_ = false;
    if (subscriberThread_.joinable()) {
        subscriberThread_.join();
    }
}

std::string RedisClient::eventToString(const Event& e) {
    return e.type + "|" + e.payload;
}

Event RedisClient::stringToEvent(const std::string& str) {
    size_t pos = str.find('|');
    if (pos == std::string::npos) return {"unknown", str};
    return {str.substr(0, pos), str.substr(pos + 1)};
}