#pragma once
#include "isub.h"

class CoinbaseSubscriber : public ISubscribeBuilder
{
   public:
    std::string build(const std::string& channel,
                      const std::vector<std::string>& products,
                      std::shared_ptr<IAuthProvider>& auth_provider) override;
};