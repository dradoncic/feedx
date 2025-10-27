#pragma once
#include <string>

class IMessageAdapter
{
   public:
    virtual void on_message(std::string_view msg) = 0;
};