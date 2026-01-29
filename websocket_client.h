#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <string>

class WebSocketClient {
public:
    explicit WebSocketClient(const std::string& url);

    void connect();
    void send_request(const std::string& message);
    std::string read_message();
    void close();

private:
    void parse_url(const std::string& url);

    boost::asio::io_context ioc_;
    boost::asio::ssl::context ctx_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ws_;

    std::string host_;
    std::string port_;
    std::string target_;
};
