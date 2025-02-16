CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -I/usr/include/c++/v1 -I.

SRCS = main.cpp jail_manager.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = installer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)