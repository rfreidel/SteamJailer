CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -I/usr/include/c++/v1
LDFLAGS =

SRCS = main.cpp iocage_installer.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = installer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)