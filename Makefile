# Simple Makefile for cache_aware_oblivious_matmul project

# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Iinclude -I./src

# Directories
SRC_DIR  := src
OBJ_DIR  := obj

# Automatically find all .cpp files in SRC_DIR
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Final executable name
TARGET   := cache_matmul

# Default target: build the executable
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Pattern rule for compiling .cpp files to .o files in the obj directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean target to remove built files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
