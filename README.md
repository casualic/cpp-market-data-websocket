# Market Data WebSocket Streamer (C++)

A minimal, reusable WebSocket client for streaming market data (Boost.Beast + OpenSSL).
Includes example connections for Alpaca (auth + subscribe) and Binance (public stream) in the main class.


## Features
- Simple `WebSocketClient` wrapper (connect / send / read / close)
- TLS support (`wss://`)
- Reusable rolling average helper for fixed-size windows, for use as reference for applying functions to stream.
- LLDB debug config and Makefile build

## Requirements
- macOS with Homebrew
- Boost
- OpenSSL

Install dependencies:
```
brew install boost openssl@3
```

## Build
```
make
```

Binary output:
```
./bin/market_data_streamer
```

## Run

Note that this is for testing with the test examples. The example Binance stream does not require authentication. 

If testing with alpaca. Create a `.env` with Alpaca credentials.
```
ALPACA_API_KEY=your_key
ALPACA_API_SECRET=your_secret
```

Run:
```
./bin/market_data_streamer
```

The flow for starting the websocket stream for the market data case is :
1. Instantiate the Client with url. (using the `WebSocketClient`).
2. Send request for authentication, (using `WebSocketClient.send_request()`). (Some times optional, depending on flow.)
3. Send subscrition message. (Again (using `WebSocketClient.send_request()`)) .
4. Once succesfully subscribed, start streaming. 

    ```cpp
    while (true) {
        const auto message = client.read_message();
    }
    ```


The main.cpp includes examples of usage (including the rolling average computation) and helper function for parsing the output.


## Example URLs
- Alpaca (auth required):
  - `wss://stream.data.alpaca.markets/v1beta3/crypto/eu-1`
- Binance (public):
  - `wss://fstream.binance.com/stream?streams=bnbusdt@aggTrade/btcusdt@markPrice`

Update the URLs in `src/main.cpp` or extend it to accept command-line args.

## Rolling Average (fixed window)
Header-only helper in `include/rolling_average.h`:
```
RollingAverage<5> avg;
if (auto value = avg.add(price)) {
    std::cout << "avg(5)=" << *value << "\n";
}
```

## Debugging (LLDB)
Press F5 in VS Code to build and debug via LLDB. The config reads `.env` automatically.

## Repo Layout
```
include/     Public headers
src/         Implementation + example main
bin/         Build output (ignored by git)
.vscode/     Debug/build tasks
```
