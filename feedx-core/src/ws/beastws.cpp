#include "ws/beastws.h"

#include <iostream>

BeastWSConnector::BeastWSConnector(boost::asio::io_context& ioc,
                                   boost::asio::ssl::context& ssl_ctx)
    : ioc_(ioc), ssl_ctx_(ssl_ctx), ws_(ioc, ssl_ctx), resolver_(ioc) {};

void BeastWSConnector::connect(const std::string& host, const std::string& port,
                               const std::vector<std::string>& channels,
                               const std::vector<std::string>& products)
{
    host_ = host;
    port_ = port;
    channels_ = channels;
    products_ = products;

    resolver_.async_resolve(host, port,
                            [this](beast::error_code ec,
                                   net::ip::tcp::resolver::results_type results)
                            { on_resolve(ec, results); });
}

void BeastWSConnector::close()
{
    ws_.async_close(websocket::close_code::normal,
                    [this](beast::error_code ec)
                    {
                        if (ec)
                            std::cerr << "Close: " << ec.message() << "\n";
                    });
}

void BeastWSConnector::send(const std::string& msg)
{
    ws_.async_write(net::buffer(msg),
                    [this](beast::error_code ec, size_t bytes_transferred)
                    {
                        if (ec)
                        {
                            if (on_error)
                                on_error("Send error: " + ec.message());
                        }
                    });
}

void BeastWSConnector::on_resolve(beast::error_code ec,
                                  ip::tcp::resolver::results_type results)
{
    if (ec)
    {
        std::cerr << "Resolve: " << ec.message() << '\n';
        return;
    }
    async_connect(ws_.next_layer().next_layer(), results.begin(),
                  results.end()),
        [this](beast::error_code ec, auto) { on_connect(ec); };
}

void BeastWSConnector::on_connect(beast::error_code ec)
{
    if (ec)
    {
        std::cerr << "Connect: " << ec.message() << '\n';
        return;
    }

    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(),
                                  host_.c_str()))
    {
        beast::error_code ec_sni{static_cast<int>(::ERR_get_error()),
                                 net::error::get_ssl_category()};
        std::cerr << "SNI: " << ec_sni.message() << "\n";
        return;
    }

    ws_.next_layer().async_handshake(net::ssl::stream_base::client,
                                     [this](beast::error_code ec)
                                     { on_ssl_handshake(ec); });
}

void BeastWSConnector::on_ssl_handshake(beast::error_code ec)
{
    if (ec)
    {
        std::cerr << "SSL Handshake error: " << ec.message() << std::endl;
        std::cerr << "Error code: " << ec.value() << std::endl;
        std::cerr << "Error category: " << ec.category().name() << std::endl;

        unsigned long ssl_err;
        while ((ssl_err = ERR_get_error()) != 0)
        {
            char err_buf[256];
            ERR_error_string_n(ssl_err, err_buf, sizeof(err_buf));
            std::cerr << "OpenSSL error: " << err_buf << std::endl;
        }
        return;
    }

    ws_.async_handshake(host_, "/",
                        [this](beast::error_code ec) { on_handshake(ec); });
}

void BeastWSConnector::on_handshake(beast::error_code ec)
{
    if (ec)
    {
        std::cerr << "Handshake: " << ec.message() << "\n";
        return;
    }

    if (heartbeat_policy_)
    {
        heartbeat_policy_->on_connected(*this);
    }

    if (subscribe_builder_)
    {
        for (const auto& channel : channels_)
        {
            subscribe(channel, products_);
        }
    }

    ws_.async_read(read_buffer_,
                   [this](beast::error_code ec, std::size_t bytes_transferred)
                   { on_read(ec, bytes_transferred); });
}

void BeastWSConnector::on_read(beast::error_code ec,
                               std::size_t bytes_transferred)
{
    if (ec)
    {
        std::cerr << "Read: " << ec.message() << "\n";
        return;
    }

    auto message = beast::buffers_to_string(read_buffer_.data());
    read_buffer_.consume(bytes_transferred);

    handle_message(message);

    ws_.async_read(read_buffer_,
                   [this](beast::error_code ec, size_t bytes_transferred)
                   { on_read(ec, bytes_transferred); });
};
