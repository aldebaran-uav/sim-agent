# Makefile for sim-agent

# Directories
BUILD_DIR = build
SRC_DIR = src

# Default target
.DEFAULT_GOAL := all

# Phony targets
.PHONY: all build clean rebuild run

# Main targets
all: build

build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && make

debug:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

rebuild: clean build

run:
	@echo "Running sim-agent..."
	@./$(BUILD_DIR)/sim-agent 5 udpin://127.0.0.1:14540