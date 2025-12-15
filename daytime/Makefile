CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET = daytime_client
SOURCES = daytime_client.cpp
OBJS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

distclean: clean
	rm -f *~

.PHONY: all clean distclean