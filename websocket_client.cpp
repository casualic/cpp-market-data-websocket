#include "websocket_client.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdexcept>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

WebSocketClient::WebSocketClient(const std::string& url)
    : ctx_(ssl::context::tls_client),
      resolver_(ioc_),
      ws_(ioc_, ctx_) {
    ctx_.set_default_verify_paths();
    ctx_.set_verify_mode(ssl::verify_peer);
    parse_url(url);
}

void WebSocketClient::parse_url(const std::string& url) {
    const std::string scheme = "wss://";
    if (url.rfind(scheme, 0) != 0) {
        throw std::runtime_error("Only wss:// URLs are supported");
    }

    std::string remainder = url.substr(scheme.size());
    std::string host_port;
    std::string path;

    const auto slash_pos = remainder.find('/');
    if (slash_pos == std::string::npos) {
        host_port = remainder;
        path = "/";
    } else {
        host_port = remainder.substr(0, slash_pos);
        path = remainder.substr(slash_pos);
        if (path.empty()) {
            path = "/";
        }
    }

    const auto colon_pos = host_port.find(':');
    if (colon_pos == std::string::npos) {
        host_ = host_port;
        port_ = "443";
    } else {
        host_ = host_port.substr(0, colon_pos);
        port_ = host_port.substr(colon_pos + 1);
        if (port_.empty()) {
            port_ = "443";
        }
    }

    if (host_.empty()) {
        throw std::runtime_error("URL host is empty");
    }

    target_ = path;
}

void WebSocketClient::connect() {
    auto const results = resolver_.resolve(host_, port_);
    net::connect(beast::get_lowest_layer(ws_), results);

    ws_.next_layer().set_verify_mode(ssl::verify_peer);
    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str())) {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }
    ws_.next_layer().handshake(ssl::stream_base::client);

    ws_.handshake(host_, target_);
}

void WebSocketClient::send_request(const std::string& message) {
    ws_.text(true);
    ws_.write(net::buffer(message));
}

std::string WebSocketClient::read_message() {
    beast::flat_buffer buffer;
    ws_.read(buffer);
    return beast::buffers_to_string(buffer.data());
}

void WebSocketClient::close() {
    beast::error_code ec;
    ws_.close(websocket::close_code::normal, ec);
}
