#include "heartbeat/channel.h"

#include <simdjson.h>

#include <string>
#include <vector>

void ChannelHeartbeat::on_connected(IWSConnector& connector)
{
    if (!heartbeat_channels_.empty())
    {
        connector.subscribe(heartbeat_channels_, {});
    }
}

void ChannelHeartbeat::on_message(IWSConnector& connector,
                                  const std::string& msg)
{
    auto [is_heartbeat, counter] = is_heartbeat_message(msg);
    if (is_heartbeat)
    {
        if (heartbeat_counter_ &&
            counter.value() != heartbeat_counter_.value() + 1)
        {
            std::cerr << "Heartbeat dropped! Expected "
                      << heartbeat_counter_.value() + 1 << " but got "
                      << counter.value() << ". Reconnecting..." << std::endl;

            connector.close();
            connector.connect(connector.host(), connector.port());
            heartbeat_counter_.reset();
            return;
        }

        heartbeat_counter_ = counter;
    }
}

std::pair<bool, std::optional<uint64_t>> ChannelHeartbeat::is_heartbeat_message(
    std::string_view msg)
{
    simdjson::padded_string json(msg);
    simdjson::ondemand::document doc = parser_.iterate(json);

    std::string_view channel = doc["channel"];
    if (!(channel == "subscriptions"))
        return {false, std::nullopt};

    simdjson::ondemand::array events = doc["events"];
    simdjson::ondemand::object event = events.at(0).get_object();

    uint64_t counter = event["heartbeat_counter"].get_uint64_in_string();
    return {true, counter};
}

void ChannelHeartbeat::add_heartbeat_channel(std::string& channel)
{
    heartbeat_channels_.push_back(channel);
}