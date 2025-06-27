CC = gcc
CFLAGS = -Wall -std=c99

LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11


RAYLIB_INCLUDE_PATH = -I/usr/include

RAYLIB_LIB_PATH = -L/usr/lib


SRC_DIR = src
BUILD_DIR = build
ASSETS_DIR = assets

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BUILD_DIR)/mygame

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CC) $(OBJS) -o $@ $(RAYLIB_LIB_PATH) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(RAYLIB_INCLUDE_PATH) -c $< -o $@ -I$(SRC_DIR)

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

.PHONY: all run clean