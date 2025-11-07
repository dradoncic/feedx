#pragma once
#include <unordered_map>

#include "imsg.h"

enum Channel
{
    TICKER,
    LEVEL2,
};

inline std::unordered_map<std::string_view, Channel> channelMap = {
    {"ticker", Channel::TICKER},
    {"l2_data", Channel::LEVEL2},
};

struct RawMessage
{
    Channel channel;
    std::string payload;
};

class CoinbaseAdapter : public IMessageAdapter
{
   public:
    void on_message(const std::string_view msg) override;

   private:
    // void on_ticker_message(const RawMessage& msg);
    // void on_orderbook_message(const RawMessage& msg);
};