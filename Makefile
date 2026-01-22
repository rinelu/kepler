PROJECT_NAME := kepler
BUILD_DIR    := build
SRC_DIR      := src
ASSETS_DIR   := assets

CC      := cc
CSTD    := -std=c11
WARN    := -Wall -Wextra -ggdb
OPT     := -O2
DEBUG   := -g

CFLAGS  := $(CSTD) $(WARN) $(OPT) $(DEBUG)
INCLUDES := -I$(SRC_DIR) -I./thirdparty/raylib-5.5_linux_amd64/include/

LIBS := -L./thirdparty/raylib-5.5_linux_amd64/lib -l:libraylib.a -lm -ldl -lpthread

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	@echo "Linking $@"
	$(CC) $(OBJS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(PROJECT_NAME)

clean:
	rm -rf $(BUILD_DIR) $(PROJECT_NAME)

rebuild: clean all

debug: CFLAGS += -DDEBUG
debug: rebuild

.PHONY: all run clean rebuild debug
