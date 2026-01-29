CXX := g++
BIN_DIR := bin
TARGET := $(BIN_DIR)/market_data_streamer
SOURCES := src/main.cpp src/websocket_client.cpp
INCLUDES := -Iinclude

CXXFLAGS := -std=c++17 -g $(INCLUDES) -I/opt/homebrew/opt/boost/include -I/opt/homebrew/opt/openssl@3/include
LDFLAGS := -L/opt/homebrew/Cellar/boost/1.90.0/lib -L/opt/homebrew/opt/openssl@3/lib
LDLIBS := -lboost_thread -lssl -lcrypto

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SOURCES) | $(BIN_DIR)
	$(CXX) $(SOURCES) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET)
