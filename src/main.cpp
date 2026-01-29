#include <chrono>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <string>

#include "rolling_average.h"
#include "websocket_client.h"

namespace {
std::optional<double> parse_number_after_key(const std::string& message, const std::string& key) {
    auto pos = message.find(key);
    if (pos == std::string::npos) {
        return std::nullopt;
    }
    pos = message.find(':', pos + key.size());
    if (pos == std::string::npos) {
        return std::nullopt;
    }
    ++pos;
    while (pos < message.size() && std::isspace(static_cast<unsigned char>(message[pos])) != 0) {
        ++pos;
    }
    if (pos >= message.size()) {
        return std::nullopt;
    }
    if (message[pos] == '"') {
        ++pos;
    }

    const auto start = pos;
    while (pos < message.size()) {
        const char ch = message[pos];
        if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-' || ch == '+' || ch == 'e' ||
            ch == 'E') {
            ++pos;
            continue;
        }
        break;
    }
    if (start == pos) {
        return std::nullopt;
    }
    return std::stod(message.substr(start, pos - start));
}

std::optional<double> extract_price(const std::string& message) {
    const std::string keys[] = {"\"p\"", "\"ap\"", "\"bp\"", "\"price\""};
    for (const auto& key : keys) {
        if (auto value = parse_number_after_key(message, key)) {
            return value;
        }
    }
    return std::nullopt;
}

std::optional<std::string> parse_string_after_key(const std::string& message, const std::string& key) {
    auto pos = message.find(key);
    if (pos == std::string::npos) {
        return std::nullopt;
    }
    pos = message.find(':', pos + key.size());
    if (pos == std::string::npos) {
        return std::nullopt;
    }
    ++pos;
    while (pos < message.size() && std::isspace(static_cast<unsigned char>(message[pos])) != 0) {
        ++pos;
    }
    if (pos >= message.size() || message[pos] != '"') {
        return std::nullopt;
    }
    ++pos;

    const auto start = pos;
    while (pos < message.size() && message[pos] != '"') {
        ++pos;
    }
    if (pos <= start || pos >= message.size()) {
        return std::nullopt;
    }
    return message.substr(start, pos - start);
}

std::optional<std::string> extract_symbol(const std::string& message) {
    const std::string keys[] = {"\"s\"", "\"S\"", "\"symbol\"", "\"stream\""};
    for (const auto& key : keys) {
        if (auto value = parse_string_after_key(message, key)) {
            if (key == "\"stream\"") {
                const auto at = value->find('@');
                if (at != std::string::npos) {
                    return value->substr(0, at);
                }
            }
            return value;
        }
    }
    return std::nullopt;
}

std::string truncate_message(const std::string& message, std::size_t limit) {
    if (message.size() <= limit) {
        return message;
    }
    return message.substr(0, limit) + "...";
}
}  // namespace

int main () {
    auto run_for_seconds = [](WebSocketClient& client, int seconds, const std::string& label) {
        std::unordered_map<std::string, RollingAverage<5>> averages;
        const auto start = std::chrono::steady_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::steady_clock::now() - start)
                   .count() < seconds) {
            const auto message = client.read_message();
            std::cout << "[" << label << "] " << truncate_message(message, 200) << std::endl;

            if (auto price = extract_price(message)) {
                const auto symbol = extract_symbol(message).value_or("unknown");
                if (auto avg = averages[symbol].add(*price)) {
                    std::cout << "[" << label << "] " << symbol << " avg(5) = " << *avg << std::endl;
                }
            }
        }
        client.close();
    };

    const int test_seconds = 5;

    {
        std::string url{"wss://stream.data.alpaca.markets/v1beta3/crypto/eu-1"};
        const char* api_key = std::getenv("ALPACA_API_KEY");
        const char* api_secret = std::getenv("ALPACA_API_SECRET");
        if (!api_key || !api_secret) {
            std::cerr << "Missing ALPACA_API_KEY or ALPACA_API_SECRET environment variable.\n";
            return 1;
        }

        WebSocketClient client(url);
        client.connect();
        client.send_request(std::string("{\"action\":\"auth\",\"key\":\"") + api_key +
                            "\",\"secret\":\"" + api_secret + "\"}");
        client.send_request("{\"action\":\"subscribe\",\"quotes\":[\"BTC/USD\"]}");
        run_for_seconds(client, test_seconds, "alpaca");
    }

    {
        std::string url{"wss://fstream.binance.com/stream?streams=bnbusdt@aggTrade/btcusdt@markPrice"};
        WebSocketClient client(url);
        client.connect();
        run_for_seconds(client, test_seconds, "binance");
    }

    return 0;
}
