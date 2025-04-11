.PHONY: all clean

# Compiler
CC := zig cc 
CFLAGS = -std=gnu11 -Wall -Wextra -Wpedantic -ffast-math -Wno-unused -lm
CFLAGS += -finline-functions -fno-strict-aliasing -funroll-loops
CFLAGS += -march=native -mtune=native -Wwrite-strings -fno-exceptions

# Add release and debug build options
CFLAGS_RELEASE = -O3 -DNDEBUG
CFLAGS_DEBUG = -g -O0 -DDEBUG

# Directories
SRC_DIR = src
BUILD_DIR = build

# Find all C source files recursively in the src directory
SRC = $(shell find $(SRC_DIR) -name '*.c')

# Object files (mirror the source directory structure in build directory)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Executable name
TARGET = $(BUILD_DIR)/main

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    # Windows
    TARGET := $(TARGET).exe
    CC = x86_64-w64-mingw32-gcc
    RM = del /Q
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Linux
        CFLAGS += -DLINUX
        RM = rm -f
    endif
    ifeq ($(UNAME_S),Darwin)
        # macOS
        CFLAGS += -DMACOS
        RM = rm -f
    endif
endif

# Ensure the build directory exists before compiling
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Default target
all: $(BUILD_DIR) release

# Release build
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(BUILD_DIR)
debug: $(TARGET)

# Debug build
release: CFLAGS += $(CFLAGS_RELEASE)
release: $(BUILD_DIR)
release: $(TARGET)

# Safe build (enables additional warnings and sanitizers)
safe: CFLAGS += -fsanitize=address -fsanitize=undefined -fstack-protector-strong -D_FORTIFY_SOURCE=2
safe: $(BUILD_DIR)
safe: $(TARGET)

# Create necessary directories and compile .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Run the program
run: all
	@./$(TARGET)

# Clean the build files
clean:
	$(RM) $(OBJ) $(TARGET)

