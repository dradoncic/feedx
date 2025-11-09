#include "msg/cbsub.h"

#include <nlohmann/json.hpp>

#include  <iostream>

std::string CoinbaseSubscriber::build(
    const std::string& channel, const std::vector<std::string>& products,
    std::shared_ptr<IAuthProvider>& auth_provider)
{
    nlohmann::json msg;
    msg["type"] = "subscribe";
    msg["channel"] = channel;
    msg["jwt"] = auth_provider->get_auth_header();
    
    if (!products.empty())
        msg["product_ids"] = products;

    return msg.dump();
}