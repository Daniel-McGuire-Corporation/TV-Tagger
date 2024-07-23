# Define the compiler and the flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Define directories
SRC_DIR = .
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj

# Define sources and objects
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

# Define the output executable
TARGET = $(BUILD_DIR)/main

# Default target to build the executable
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
