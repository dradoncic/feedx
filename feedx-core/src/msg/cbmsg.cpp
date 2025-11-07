#include "msg/cbmsg.h"


void CoinbaseAdapter::on_message(const std::string_view msg)
{
    std::cout << "Recieved: " << msg << std::endl;
}