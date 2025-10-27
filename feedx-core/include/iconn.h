#pragma once
#include <string>
#include "iws.h"

class IHeartbeatPolicy
{
public:
    virtual ~IHeartbeatPolicy() = default;

    virtual void on_message(IWSConnector& connector, const std::string& msg) = 0;

    virtual void on_connected(IWSConnector& connector) = 0;

    virtual void tick(IWSConnector& connector) = 0;
};