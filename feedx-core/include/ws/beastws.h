#pragma once
#include <simdjson.h>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>

#include "iws.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ip = boost::asio::ip;

class BeastWSConnector : public IWSConnector
{
   public:
    BeastWSConnector(boost::asio::io_context& ioc,
                                       boost::asio::ssl::context& ssl_ctx);

    void connect(const std::string& host, const std::string& port,
                 const std::vector<std::string>& channels,
                 const std::vector<std::string>& products) override;
    void close() override;
    void send(const std::string& msg) override;

   private:
    void on_resolve(beast::error_code ec,
                    ip::tcp::resolver::results_type results);
    void on_connect(beast::error_code ec);
    void on_ssl_handshake(beast::error_code ec);
    void on_handshake(beast::error_code ec);
    void on_read(beast::error_code ec, size_t bytes_transferred);

    net::io_context& ioc_;  // event loop, i.e. schedules and executes all of
                            // the asynchronous network operations
    net::ssl::context& ssl_ctx_;  // tls configuration context
    websocket::stream<beast::ssl_stream<ip::tcp::socket>>
        ws_;                      // websocket object
    ip::tcp::resolver resolver_;  // resolves domains

    beast::flat_buffer read_buffer_;  // internally managed buffer
    std::string write_buffer_;        // temporary write buffer

    std::string host_;
    std::string port_;
    std::vector<std::string> products_;
    std::string channel_;
};