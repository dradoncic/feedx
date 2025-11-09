#include "msg/pmmsg.h"


void PolymarketAdapter::on_message(const std::string_view msg)
{
    std::cout << "Recieved: " << msg << std::endl;
}