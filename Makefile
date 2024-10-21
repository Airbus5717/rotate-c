.PHONY: all clean

# Compiler
CC := zig cc 
CFLAGS = -std=gnu11 -Wall -Wextra -Wpedantic -ffast-math -Wno-unused
CFLAGS += -finline-functions -fno-strict-aliasing -funroll-loops
CFLAGS += -march=native -mtune=native -Wwrite-strings -fno-exceptions

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

# Create necessary directories and compile .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build the final executable
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)


run: all
	@./$(TARGET)
# Clean the build files
clean:
	$(RM) $(OBJ) $(TARGET)

