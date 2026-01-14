CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Wno-unused-parameter
CFLAGS_DEBUG = -Wall -Wextra -std=c99 -g -O0 -DDEBUG -Wno-unused-parameter
CFLAGS_RELEASE = -Wall -Wextra -std=c99 -O2 -DNDEBUG -Wno-unused-parameter
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
LDFLAGS_RELEASE = -lraylib -lopengl32 -lgdi32 -lwinmm -s

SRC_DIR = src
BUILD_DIR = build
BUILD_DIR_DEBUG = build/debug
BUILD_DIR_RELEASE = build/release

TARGET = $(BUILD_DIR)/game.exe
TARGET_DEBUG = $(BUILD_DIR_DEBUG)/game_debug.exe
TARGET_RELEASE = $(BUILD_DIR_RELEASE)/game.exe
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
OBJ_DEBUG = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR_DEBUG)/%.o,$(SRC))
OBJ_RELEASE = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR_RELEASE)/%.o,$(SRC))

all: $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR_DEBUG):
	@mkdir -p $(BUILD_DIR_DEBUG)

$(BUILD_DIR_RELEASE):
	@mkdir -p $(BUILD_DIR_RELEASE)

$(TARGET): $(BUILD_DIR) $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(BUILD_DIR_DEBUG) $(OBJ_DEBUG)
	$(CC) $(OBJ_DEBUG) -o $(TARGET_DEBUG) $(LDFLAGS)

$(BUILD_DIR_DEBUG)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR_DEBUG)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(BUILD_DIR_RELEASE) $(OBJ_RELEASE)
	$(CC) $(OBJ_RELEASE) -o $(TARGET_RELEASE) $(LDFLAGS_RELEASE)

$(BUILD_DIR_RELEASE)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR_RELEASE)
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

run: $(TARGET)
	@$(TARGET)

run-debug: $(TARGET_DEBUG)
	@$(TARGET_DEBUG)

run-release: $(TARGET_RELEASE)
	@$(TARGET_RELEASE)

.PHONY: all clean run debug run-debug release run-release
