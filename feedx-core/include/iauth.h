#pragma once
#include <map>
#include <string>

class IAuthProvider
{
public:
    virtual std::string get_auth_message() const { return ""; }
};