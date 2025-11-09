#pragma once
#include <unordered_map>

#include "imsg.h"

class PolymarketAdapter : public IMessageAdapter
{
   public:
    void on_message(const std::string_view msg) override;
};