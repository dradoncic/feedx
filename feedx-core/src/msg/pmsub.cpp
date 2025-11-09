#include "msg/pmsub.h"

#include <nlohmann/json.hpp>

#include  <iostream>

std::string PolymarketSubscriber::build(
    const std::string& channel, const std::vector<std::string>& products,
    std::optional<std::shared_ptr<IAuthProvider>> auth_provider)
{
    nlohmann::json msg;
    msg["topic"] = "clob_market";
    msg["type"] = channel;
    if (!products.empty())
        msg["filters"] = products;

    return msg.dump();
}