CXX := g++
TARGET := main
SOURCES := main.cpp websocket_client.cpp

CXXFLAGS := -std=c++17 -g -I/opt/homebrew/opt/boost/include -I/opt/homebrew/opt/openssl@3/include
LDFLAGS := -L/opt/homebrew/Cellar/boost/1.90.0/lib -L/opt/homebrew/opt/openssl@3/lib
LDLIBS := -lboost_thread -lssl -lcrypto

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(SOURCES) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET)
