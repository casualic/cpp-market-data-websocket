#include <cmath>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <vector>
#include <chrono>
#include "websocket_client.h"

int main () {
    auto run_for_seconds = [](WebSocketClient& client, int seconds) {
        const auto start = std::chrono::steady_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::steady_clock::now() - start)
                   .count() < seconds) {
            std::cout << client.read_message() << std::endl;
        }
        client.close();
    };

    const int test_seconds = 10;

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
        run_for_seconds(client, test_seconds);
    }

    {
        std::string url{"wss://fstream.binance.com/stream?streams=bnbusdt@aggTrade/btcusdt@markPrice"};
        WebSocketClient client(url);
        client.connect();
        run_for_seconds(client, test_seconds);
    }

    return 0;
}
