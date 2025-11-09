#include "heartbeat/ping.h"

#include "ws/iws.h"

PingHeartbeat::PingHeartbeat(std::chrono::seconds interval)
    : interval_{interval}, last_ping_time_{std::nullopt}
{
}

void PingHeartbeat::on_connected(IWSConnector& connector)
{
    last_ping_time_ = std::chrono::steady_clock::now();
}

void PingHeartbeat::on_message(IWSConnector& connector,
                               const std::string_view msg)
{
    auto now = std::chrono::steady_clock::now();

    if (!last_ping_time_.has_value())
    {
        last_ping_time_ = now;
        return;
    }

    auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(
        now - last_ping_time_.value());

    if (elapsed_time >= interval_)
    {
        std::string ping = R"({"type": "ping"})";
        connector.send(ping);
        last_ping_time_ = now;
    }
}