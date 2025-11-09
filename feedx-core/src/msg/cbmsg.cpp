#include "msg/cbmsg.h"


void CoinbaseAdapter::on_message(const std::string_view msg)
{
    simdjson::padded_string json(msg);
    simdjson::ondemand::document doc = parser_.iterate(json);

    std::string_view channel = doc["channel"];
    if (channel == "subscriptions" || channel == "heartbeats")
        return;

    std::cout << "Recieved: " << msg << std::endl;
}