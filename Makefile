# Rotate Programming Language Compiler Makefile
# Author: [Auto-generated]
# Description: Build system for the Rotate C compiler

.PHONY: all clean debug release safe run test help install uninstall format check

# Project Information
PROJECT_NAME = rotate
VERSION = 0.0.1

# Compiler Configuration
CC := clang 
CFLAGS = -std=gnu11 -Wall -Wextra -Wpedantic -ffast-math -Wno-unused
LDFLAGS = -lm
CFLAGS += -finline-functions -fno-strict-aliasing -funroll-loops
CFLAGS += -march=native -mtune=native -Wwrite-strings -fno-exceptions
CFLAGS += -Wshadow -Wundef -Wcast-align -Wstrict-prototypes
CFLAGS += -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations

# Build Configuration
CFLAGS_RELEASE = -O3 -DNDEBUG -flto
CFLAGS_DEBUG = -g -O1 -DDEBUG -fsanitize=address -fsanitize=undefined
CFLAGS_SAFE = -fsanitize=address -fsanitize=undefined -fstack-protector-strong -D_FORTIFY_SOURCE=2

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = test
INSTALL_PREFIX ?= /usr/local

# Source Files
SRC = $(shell find $(SRC_DIR) -name '*.c' -type f)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))
DEPS = $(OBJ:.o=.d)

# Executable Configuration
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)
TEST_FILES = $(wildcard $(TEST_DIR)/*.vr)

# Platform Detection and Configuration
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    ifeq ($(shell which x86_64-w64-mingw32-gcc 2>/dev/null),)
        CC := gcc
    else
        CC := x86_64-w64-mingw32-gcc
    endif
    RM := if exist build rmdir /s /q build
    MKDIR := if not exist
    PATHSEP := \\
    CFLAGS += -DOS_WIN
    LDFLAGS += -lmsvcrt
else
    UNAME_S := $(shell uname -s)
    RM := rm -rf
    MKDIR := mkdir -p
    PATHSEP := /
    ifeq ($(UNAME_S),Linux)
        CFLAGS += -DOS_LIN
    endif
    ifeq ($(UNAME_S),Darwin)
        CFLAGS += -DOS_MAC
    endif
endif

# Color Output
NO_COLOR := \033[0m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m
RED := \033[31m

# Build Targets
# all: debug 
# 	@printf "$(GREEN)✓ Build completed successfully$(NO_COLOR)\n"
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(TARGET)
	@printf "$(YELLOW)✓ Debug build complete$(NO_COLOR)\n"

release: CFLAGS += $(CFLAGS_RELEASE)
release: $(TARGET)
	@printf "$(GREEN)✓ Release build complete$(NO_COLOR)\n"


safe: CFLAGS += $(CFLAGS_SAFE)
safe: $(TARGET)
	@printf "$(BLUE)✓ Safe build complete$(NO_COLOR)\n"

# Dependency Generation
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@$(MKDIR) $(dir $@) 2>/dev/null || true
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@ 2>/dev/null || true

# Object File Compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)/%.d | $(BUILD_DIR)
	@$(MKDIR) $(dir $@) 2>/dev/null || true
	@printf "$(BLUE)Compiling$(NO_COLOR) %s\n" "$<"
	@$(CC) $(CFLAGS) -c $< -o $@ || (printf "$(RED)Error compiling $<$(NO_COLOR)\n" && exit 1)

# Executable Linking
$(TARGET): $(OBJ) | $(BUILD_DIR)
	@printf "$(YELLOW)Linking$(NO_COLOR) %s\n" "$@"
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) || (printf "$(RED)Error linking $@$(NO_COLOR)\n" && exit 1)

# Directory Creation
$(BUILD_DIR):
	@$(MKDIR) $(BUILD_DIR) 2>/dev/null || true

# Test Execution
test: debug $(TARGET) 
	@printf "$(BLUE)Running tests...$(NO_COLOR)\n"
	@if [ -d "$(TEST_DIR)" ]; then \
		passed=0; failed=0; total=0; \
		for test_file in $(TEST_FILES); do \
			total=$$((total + 1)); \
			printf "Testing $$(basename $$test_file)... "; \
			if ./$(TARGET) $$test_file >/dev/null 2>&1; then \
				printf "$(GREEN)PASS$(NO_COLOR)\n"; \
				passed=$$((passed + 1)); \
			else \
				printf "$(RED)FAIL$(NO_COLOR)\n"; \
				failed=$$((failed + 1)); \
			fi; \
		done; \
		echo "========================================"; \
		printf "Total: $$total  $(GREEN)Passed: $$passed$(NO_COLOR)  $(RED)Failed: $$failed$(NO_COLOR)\n"; \
		if [ $$failed -eq 0 ]; then \
			printf "$(GREEN)All tests passed!$(NO_COLOR)\n"; \
		else \
			printf "$(RED)Some tests failed.$(NO_COLOR)\n"; \
			exit 1; \
		fi; \
	else \
		printf "$(YELLOW)No test directory found$(NO_COLOR)\n"; \
	fi

# Zig Test Execution
test-zig:
	@printf "$(BLUE)Running Zig tests...$(NO_COLOR)\n"
	@if [ -f "$(SRC_DIR)/test.zig" ]; then \
		zig test $(SRC_DIR)/test.zig; \
	else \
		printf "$(YELLOW)No Zig test file found$(NO_COLOR)\n"; \
	fi

# Run Compiler
run: $(TARGET)
	@printf "$(GREEN)Running$(NO_COLOR) %s\n" "$(TARGET)"
	@./$(TARGET) $(ARGS)

# Format Code (if clang-format is available)
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		printf "$(BLUE)Formatting code...$(NO_COLOR)\n"; \
		find $(SRC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
		printf "$(GREEN)✓ Code formatted$(NO_COLOR)\n"; \
	else \
		printf "$(YELLOW)clang-format not found, skipping formatting$(NO_COLOR)\n"; \
	fi

# Static Analysis
check:
	@if command -v cppcheck >/dev/null 2>&1; then \
		printf "$(BLUE)Running static analysis...$(NO_COLOR)\n"; \
		cppcheck --enable=all --inconclusive --std=c11 $(SRC_DIR); \
	else \
		printf "$(YELLOW)cppcheck not found, skipping static analysis$(NO_COLOR)\n"; \
	fi

# Installation
install: release
	@printf "$(BLUE)Installing %s...$(NO_COLOR)\n" "$(PROJECT_NAME)"
	@$(MKDIR) $(INSTALL_PREFIX)/bin
	@cp $(TARGET) $(INSTALL_PREFIX)/bin/
	@printf "$(GREEN)✓ Installed to %s/bin/$(NO_COLOR)\n" "$(INSTALL_PREFIX)"

# Uninstallation
uninstall:
	@printf "$(BLUE)Uninstalling %s...$(NO_COLOR)\n" "$(PROJECT_NAME)"
	@$(RM) $(INSTALL_PREFIX)/bin/$(PROJECT_NAME)*
	@printf "$(GREEN)✓ Uninstalled$(NO_COLOR)\n"

# Clean Build Artifacts
clean:
	@printf "$(RED)Cleaning build artifacts...$(NO_COLOR)\n"
	@$(RM) $(BUILD_DIR)
	@printf "$(GREEN)✓ Clean complete$(NO_COLOR)\n"

# Show Help Information
help:
	@printf "$(GREEN)Rotate Compiler Build System$(NO_COLOR)\n"
	@echo "Version: $(VERSION)"
	@echo ""
	@printf "$(BLUE)Available targets:$(NO_COLOR)\n"
	@echo "  all        - Build release version (default)"
	@echo "  release    - Build optimized release version"
	@echo "  debug      - Build debug version with sanitizers"
	@echo "  safe       - Build with additional safety checks"
	@echo "  run        - Build and run compiler (use ARGS=... for arguments)"
	@echo "  test       - Run .vr test files through compiler"
	@echo "  test-zig   - Run Zig unit tests"
	@echo "  format     - Format source code with clang-format"
	@echo "  check      - Run static analysis with cppcheck"
	@echo "  install    - Install binary to $(INSTALL_PREFIX)/bin"
	@echo "  uninstall  - Remove installed binary"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Show this help message"
	@echo ""
	@printf "$(BLUE)Examples:$(NO_COLOR)\n"
	@echo "  make run ARGS='test/001_hello.vr --lex'"
	@echo "  make debug"
	@echo "  make install INSTALL_PREFIX=/opt/rotate"

# Include dependency files
-include $(DEPS)

