#pragma once
#include <simdjson.h>

#include <chrono>
#include <optional>
#include <string>

#include "iheartbeat.h"

class PingHeartbeat : public IHeartbeatPolicy
{
   public:
    PingHeartbeat(std::chrono::seconds interval = std::chrono::seconds(10));

    void on_message(IWSConnector& connector,
                    const std::string_view msg) override;
    void on_connected(IWSConnector& connector) override;

   private:
    simdjson::ondemand::parser parser_;
    std::chrono::seconds interval_;
    std::optional<std::chrono::steady_clock::time_point> last_ping_time_;
};