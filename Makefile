CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I include -O2
SRC      = src/main.cpp src/Account.cpp src/Transaction.cpp src/FileHandler.cpp
TARGET   = banking_server

# Windows needs to link the sockets library; Linux/Mac do not.
ifeq ($(OS),Windows_NT)
    LIBS = -lws2_32
else
    LIBS = -lpthread
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET) $(TARGET).exe

run: all
	./$(TARGET)
