PROJECT_NAME := kepler
BUILD_DIR := build

.PHONY: all configure build run clean rebuild debug

all: build

configure:
	@mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/$(PROJECT_NAME)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build

debug:
	@mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR)
