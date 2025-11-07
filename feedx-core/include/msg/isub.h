#pragma once
#include <string>
#include <vector>

#include "../auth/iauth.h"

class ISubscribeBuilder
{
   public:
    virtual ~ISubscribeBuilder() = default;

    virtual std::string build(
        const std::string& channel, const std::vector<std::string>& products,
        std::shared_ptr<IAuthProvider>& auth_provider) = 0;
};