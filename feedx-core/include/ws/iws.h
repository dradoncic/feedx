#pragma once
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <string>

#include "../auth/iauth.h"
#include "../msg/imsg.h"
#include "../msg/isub.h"

class IHeartbeatPolicy;

class IWSConnector
{
   public:
    virtual ~IWSConnector() = default;

    virtual void connect(const std::string& url,
                         const std::string& port = "443",
                         const std::vector<std::string>& channels = {},
                         const std::vector<std::string>& products = {}) = 0;

    virtual void close() = 0;

    virtual void send(const std::string& msg) = 0;

    std::function<void()> on_connected;
    std::function<void()> on_disconnected;
    std::function<void(const std::string&)> on_error;

    void subscribe(const std::string& channel,
                           const std::vector<std::string>& products);

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

    void set_message_adapter(std::shared_ptr<IMessageAdapter> adapter)
    {
        message_adapter_ = std::move(adapter);
    }

    const std::string& host()
    {
        return host_;
    }
    const std::string& port()
    {
        return port_;
    }
    const std::vector<std::string>& channels()
    {
        return channels_;
    }
    const std::vector<std::string>& products()
    {
        return products_;
    }

   protected:
    std::string host_;
    std::string port_;
    std::vector<std::string> channels_;
    std::vector<std::string> products_;

    std::shared_ptr<IHeartbeatPolicy> heartbeat_policy_;
    std::shared_ptr<IAuthProvider> auth_provider_;
    std::shared_ptr<ISubscribeBuilder> subscribe_builder_;
    std::shared_ptr<IMessageAdapter> message_adapter_;

    void handle_message(const std::string_view msg);
    void handle_disconnect();
};