CC = gcc
CFLAGS = -Wall -Wextra -std=c99
CFLAGS_DEBUG = -Wall -Wextra -std=c99 -g -O0 -DDEBUG
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

SRC_DIR = src
BUILD_DIR = build
BUILD_DIR_DEBUG = build/debug

TARGET = $(BUILD_DIR)/game.exe
TARGET_DEBUG = $(BUILD_DIR_DEBUG)/game_debug.exe
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
OBJ_DEBUG = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR_DEBUG)/%.o,$(SRC))

all: $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR_DEBUG):
	@mkdir -p $(BUILD_DIR_DEBUG)

$(TARGET): $(BUILD_DIR) $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(BUILD_DIR_DEBUG) $(OBJ_DEBUG)
	$(CC) $(OBJ_DEBUG) -o $(TARGET_DEBUG) $(LDFLAGS)

$(BUILD_DIR_DEBUG)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR_DEBUG)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

run: $(TARGET)
	@$(TARGET)

run-debug: $(TARGET_DEBUG)
	@$(TARGET_DEBUG)

.PHONY: all clean run debug run-debug
