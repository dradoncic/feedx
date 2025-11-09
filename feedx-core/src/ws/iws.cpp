#include "ws/iws.h"

#include "heartbeat/iheartbeat.h"

void IWSConnector::subscribe(const std::string& channel,
                             const std::vector<std::string>& products)
{
    if (!subscribe_builder_)
    {
        return;
    }
    auto msg = subscribe_builder_->build(channel, products, auth_provider_);
    send(msg);
}

void IWSConnector::handle_message(const std::string_view msg)
{
    if (heartbeat_policy_)
        heartbeat_policy_->on_message(*this, msg);
    if (message_adapter_)
        message_adapter_->on_message(msg);
}

void IWSConnector::handle_disconnect()
{
    if (on_disconnected)
        on_disconnected();
}