#pragma once
#include "isub.h"

class PolymarketSubscriber : public ISubscribeBuilder
{
   public:
    std::string build(const std::string& channel,
                      const std::vector<std::string>& products,
                      std::optional<std::shared_ptr<IAuthProvider>> auth_provider) override;
};