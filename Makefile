# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -O2
DEBUGFLAGS = -std=c++20 -Wall -Wextra -g

# Target executable
TARGET = compilador

# Source files
SOURCES = main.cpp utils.cpp

# Header files
HEADERS = compiler.hpp utils.hpp

# Default target
all: $(TARGET)

# Build the executable directly
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Debug build
debug: CXXFLAGS = $(DEBUGFLAGS)
debug: clean $(TARGET)

# Clean all generated files including test outputs
clean: clean
	rm -f *.pre *.o1 *.o2 *.err
