#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <csignal>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "auth/cbauth.h"
#include "heartbeat/channel.h"
#include "msg/cbmsg.h"
#include "msg/cbsub.h"
#include "ws/beastws.h"

static std::atomic<bool> g_running{true};

static void handle_signal(int)
{
    g_running.store(false, std::memory_order_relaxed);
}

int main(int argc, char** argv)
{
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    net::io_context ioc;
    net::ssl::context ssl_ctx(net::ssl::context::tls_client);

    ssl_ctx.set_options(
        net::ssl::context::default_workarounds | net::ssl::context::no_sslv2 |
        net::ssl::context::no_sslv3 | net::ssl::context::no_tlsv1 |
        net::ssl::context::no_tlsv1_1);

    ssl_ctx.set_default_verify_paths();
    ssl_ctx.set_verify_mode(net::ssl::verify_peer);

    SSL_CTX_set_tlsext_servername_callback(ssl_ctx.native_handle(), nullptr);

    auto ws = std::make_shared<BeastWSConnector>(ioc, ssl_ctx);

    auto auth_provider = std::make_shared<CoinbaseAuth>();
    ws->set_auth_policy(auth_provider);

    auto subscribe_builder = std::make_shared<CoinbaseSubscriber>();
    ws->set_subscribe_builder(subscribe_builder);

    auto message_adapter = std::make_shared<CoinbaseAdapter>();
    ws->set_message_adapter(message_adapter);

    auto heartbeat_policy = std::make_shared<ChannelHeartbeat>();
    std::string heartbeat_channel = "heartbeats";
    heartbeat_policy->add_heartbeat_channel(heartbeat_channel);
    ws->set_heartbeat_policy(heartbeat_policy);

    ws->on_connected = []()
    { std::cout << "Connected to Coinbase Websocket" << std::endl; };

    ws->on_disconnected = []()
    { std::cout << "Disconnected from Coinbase Websocket" << std::endl; };

    ws->on_error = [](const std::string& err)
    { std::cout << "Error: " << err << std::endl; };

    std::vector<std::string> products = {"BTC-USD", "ETH-USD"};
    std::vector<std::string> channels = {"level2"};

    std::string host = "advanced-trade-ws.coinbase.com";
    std::string port = "443";

    ws->connect(host, port, channels, products);

    std::cout << "Connecting to " << host << ":" << port << std::endl;
    std::cout << "Subscribing to channels: level2" << std::endl;
    std::cout << "Products: BTC-USD, ETH-USD" << std::endl;
    
    std::thread io_thread(
        [&ioc]()
        {
            try
            {
                ioc.run();
            }
            catch (const std::exception& e)
            {
                std::cerr << "[io_thread] Exception: " << e.what() << "\n";
            }
        });

    while (g_running.load(std::memory_order_relaxed))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ws->close();
    ioc.stop();
    if (io_thread.joinable())
        io_thread.join();

    return 0;
}