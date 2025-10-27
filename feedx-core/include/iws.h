#pragma once
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <string>

#include "iauth.h"
#include "iheartbeat.h"
#include "isub.h"

class IWSConnector
{
   public:
    virtual ~IWSConnector() = default;

    virtual void connect(const std::string& url,
                         const std::string& port = "443") = 0;

    virtual void close() = 0;

    virtual void send(const std::string& msg) = 0;

    std::function<void()> on_connected;
    std::function<void()> on_disconnected;
    std::function<void(const std::string&)> on_message;
    std::function<void(const std::string&)> on_error;

    virtual void subscribe(const std::string& channel,
                           const std::vector<std::string>& products)
    {
        if (!subscribe_builder_)
        {
            return;
        }
        auto msg = subscribe_builder_->build(channel, products, auth_provider_);
        send(msg);
    };

    virtual void subscribe(const std::vector<std::string>& channels,
                           const std::vector<std::string>& products)
    {
        if (!subscribe_builder_)
        {
            return;
        }
        auto msg =
            subscribe_builder_->build(channels, products, auth_provider_);
        send(msg);
    };

    void set_heartbeat_policy(std::shared_ptr<IHeartbeatPolicy> policy)
    {
        heartbeat_policy_ = std::move(policy);
    }

    void set_auth_policy(std::shared_ptr<IAuthProvider> provider)
    {
        auth_provider_ = std::move(provider);
    }

    void set_subscribe_builder(std::shared_ptr<ISubscribeBuilder> builder)
    {
        subscribe_builder_ = std::move(builder);
    }

    const std::string& host() { return host_; }
    const std::string& port() { return port_; }

   protected:
    std::string host_;
    std::string port_;

    std::shared_ptr<IHeartbeatPolicy> heartbeat_policy_;
    std::shared_ptr<IAuthProvider> auth_provider_;
    std::shared_ptr<ISubscribeBuilder> subscribe_builder_;

    void handle_message(const std::string& msg)
    {
        if (heartbeat_policy_)
            heartbeat_policy_->on_message(*this, msg);
        if (on_message)
            on_message(msg);
    }

    void handle_disconnect()
    {
        if (on_disconnected)
            on_disconnected();
    }
};