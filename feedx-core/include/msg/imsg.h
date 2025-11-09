#pragma once
#include <simdjson.h>

#include <string>
class IMessageAdapter
{
   public:
    virtual void on_message(const std::string_view msg) = 0;

   protected:
    simdjson::ondemand::parser parser_;
};