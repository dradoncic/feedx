#pragma once
#include <string>

#include "../ws/iws.h"

class IHeartbeatPolicy
{
   public:
    virtual ~IHeartbeatPolicy() = default;

    virtual void on_message(IWSConnector& connector,
                            const std::string& msg) = 0;

    virtual void on_connected(IWSConnector& connector) = 0;
};