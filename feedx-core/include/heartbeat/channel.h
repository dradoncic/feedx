#pragma once
#include <simdjson.h>

#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include "iheartbeat.h"

class ChannelHeartbeat : public IHeartbeatPolicy
{
   public:
    void on_message(IWSConnector& connector, const std::string_view& msg) override;
    void on_connected(IWSConnector& connector) override;

    std::pair<bool, std::optional<uint64_t>> is_heartbeat_message(
        std::string_view msg);

    void add_heartbeat_channel(std::string& channel);

   private:
    simdjson::ondemand::parser parser_;

    std::vector<std::string> heartbeat_channels_;
    std::optional<uint64_t> heartbeat_counter_;
};