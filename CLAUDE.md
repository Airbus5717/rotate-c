# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Rotate is a programming language compiler written in C that compiles source files with `.vr` extension. This is the C implementation of the Rotate compiler, which is currently in early development (version 0.0.1) and supports lexical analysis with parsing partially implemented.

## Build Commands

The project uses a Makefile with Zig's C compiler (`zig cc`) as the default compiler:

- `make` or `make all` - Build release version (optimized)
- `make debug` - Build debug version with symbols and debugging info
- `make release` - Build optimized release version explicitly  
- `make safe` - Build with address sanitizer and additional safety checks
- `make run` - Build and run the compiler
- `make clean` - Remove build artifacts

Build outputs go to `build/` directory with the main executable at `build/rotate` (changed from `build/main` in recent updates).

## Testing

- Test files are located in `test/` directory with `.vr` extension
- Run tests with the compiler: `./build/rotate test/001_hello.vr` or use `make test` to run all test files
- Additional test targets: `make test-zig` for Zig unit tests
- There's a Zig test file at `src/test.zig` - run with `zig test src/test.zig`

## Compiler Usage

```bash
./build/rotate <source_file.vr> [options]
```

Available options:
- `--lex` - Perform lexical analysis only
- `--log` - Dump compilation info as orgmode format to `output.org`
- `--timer` - Show timing information
- `--version` or `-v` - Show version information

## Code Architecture

### Compilation Pipeline

The compiler follows a traditional multi-stage compilation process defined in `src/compile.c`:

1. **File Reading** (`ST_FILE`) - Read source file into memory
2. **Lexical Analysis** (`ST_LEXER`) - Tokenize source code using `src/fe/lexer.c`
3. **Parsing** (`ST_PARSER`) - Convert tokens to AST (partially implemented, can be skipped with `--lex` flag)
4. **Type Checking** (`ST_TCHECKER`) - Not yet implemented
5. **Logging** (`ST_LOGGER`) - Optional debug output generation

### Key Components

- **Frontend (`src/fe/`)**:
  - `lexer.c/.h` - Complete tokenization engine with comprehensive token types
  - `parser.c/.h` - AST generation (basic structure implemented, partially functional)
  - `token.c/.h` - Token definitions and utilities
  - `type.c/.h` - Type system definitions and AST node types

- **Core Systems (`src/`)**:
  - `main.c` - Entry point, command line parsing, timing, and error reporting
  - `compile.c` - Main compilation orchestration with multi-stage pipeline
  - `file.c` - File I/O utilities with error handling
  - `log.c` - Logging and debug output with color-coded messages

- **Utilities (`src/utl/`)**:
  - `common.c` - Shared utility functions

- **Headers (`src/include/`)**:
  - `defines.h` - Core type definitions, macros, platform detection, and extensive assertion system
  - `common.h` - Shared function declarations
  - `arraylist.h` - Sophisticated generic dynamic array and slice system
  - `mem.h` - Memory management utilities
  - `compile.h` - Compilation pipeline definitions
  - `file.h` - File I/O function declarations
  - `log.h` - Logging function declarations

### Token System

The lexer supports a comprehensive set of tokens defined in `src/fe/token.h`, including:
- Keywords (fn, let, if, else, while, for, struct, enum, etc.)
- Literals (integers, floats, strings, chars, booleans)
- Operators (arithmetic, logical, bitwise, comparison)
- Delimiters (parentheses, brackets, braces)

### Language Features

Based on the test files and current implementation, Rotate supports:
- **Functions** with type annotations (`fn(x: int, y: int) int`)
- **Variables** (`:=`) and **constants** (`::`) with type inference
- **Control flow**: if/else, while loops, for loops, switch statements
- **Data structures**: structs with field access and array syntax
- **Enums** for enumerated types
- **Import system** for standard libraries (`io :: import "std/io"`)
- **Memory management** with `new`/`delete` and `defer` (planned)
- **Array types** with bracket notation (`[3]int`)

### Error Handling

The codebase uses extensive assertion macros defined in `defines.h`:
- `ASSERT(expr, msg)` - Runtime assertions that exit on failure
- `ASSERT_CMP(expr1, expr2, msg)` - Compare two expressions
- `ASSERT_NULL(expr, msg)` - Check for null pointers
- `STR_ASSERT(string1, string2, msg)` - String comparison assertions
- `ASSERT_RET_FAIL(expr, msg)` - Assertions that return failure status instead of exiting
- `TODO(str)` - Mark unimplemented features with yellow warning
- `UNREACHABLE()` - Mark code paths that should never execute
- `expect(expr, doWhenExpected, doElse)` - Conditional execution macro
- Color-coded error messages using terminal color codes

### Array and Memory Management System

The project implements a sophisticated generic array and slice system through `arraylist.h`:

#### Array Types and Generation
- **Generic array creation**: `generate_array_type(T)` creates `Array(T)` and `Slice(T)` types
- **Type-safe arrays**: Each array type is specific to its element type (e.g., `Array_int`, `Array_Token`)
- **Flexible element arrays**: Arrays use flexible array members for efficient memory layout

#### Array Operations
- **Creation**: `array_make(T, size)` - Allocate new array with initial capacity
- **Element access**: `array_at(arr, idx)` - Access element by index
- **Dynamic growth**: `array_push(arr, value)` - Add element with automatic capacity doubling
- **Iteration**: `array_for_each(arr, el)` - Type-safe iteration over elements
- **Bounds**: `array_start(arr)`, `array_end(arr)` - Get pointer boundaries
- **Cleanup**: `array_free(arr)` - Release array memory

#### Slice System
- **Lightweight views**: `Slice(T)` provides non-owning views into arrays
- **Slice creation**: `slice_make(T, start, count)` and `slice_from_array(arr, low, high)`
- **Sequence operations**: `seq_start()`, `seq_end()`, `seq_length()` work on both arrays and slices
- **Iteration**: `for_each(seq, iter)` provides uniform iteration over sequences

#### Memory Layout
- **Header structure**: Arrays include `Array_Header` with count and capacity
- **Automatic resizing**: Capacity doubles when array grows beyond current limit
- **Manual memory management**: Explicit allocation/deallocation required
- **Resource cleanup**: Handled in compilation pipeline

## Coding Style and Conventions

This codebase follows distinctive coding patterns that should be maintained:

### Function and Variable Naming
- **Snake_case for types**: `TknType`, `LexErr`, `Array_Header`
- **lowercase_underscore for functions**: `lexer_init()`, `file_read()`, `log_error()`
- **lowercase_underscore for variables**: `file_size`, `token_count`, `begin_tkn_line`
- **ALL_CAPS for constants**: `MAX_IDENTIFIER_LENGTH`, `RUINT_MAX`, `SUCCESS`

### Function Declaration Style
- Return type on separate line for function definitions:
```c
File
file_read(cstr name)
{
    // implementation
}
```
- Parameters aligned and clearly typed
- Use `cstr` typedef for const char* consistently

### Custom Type System
- **Extensive typedef usage**: `u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64` with compile-time size validation
- **Float types**: `f32`, `f64`, `f128` (f128 is platform-dependent, warned as hardware-dependent)
- **Size types**: `usize`, `isize` (both 64-bit aligned, equivalent to u64/i64)
- **Convenience types**: `cstr` for const char*, `uint` for u32
- **Status codes**: `SUCCESS` (0), `FAILURE` (1), `DONE` (2)
- **Project constants**: `RTVERSION` ("0.0.1"), `RUINT_MAX`, `EXTRA_NULL_TERMINATORS`

### Macro-Heavy Architecture
- **Generic arrays**: Use `generate_array_type(T)` to create type-specific `Array(T)` and `Slice(T)`
- **Sequence operations**: `seq_start()`, `seq_end()`, `seq_length()` work uniformly on arrays and slices
- **Array operations**: `array_make()`, `array_push()`, `array_at()`, `array_free()`, `array_for_each()`
- **Slice operations**: `slice_make()`, `slice_from_array()`, `for_each()` for iteration
- **Type safety**: `seq_elem_type(seq)` and `typeof()` ensure compile-time type checking
- **Visibility control**: `internal` macro for static functions

### Error Handling Patterns
- **Assertion macros**: `ASSERT()`, `ASSERT_NULL()`, `ASSERT_RET_FAIL()`
- **Status returns**: Functions return `u8` status codes (`SUCCESS`, `FAILURE`)
- **Centralized logging**: Timestamped, color-coded log functions
- **Early returns**: Validate inputs early and return failure status

### Memory Management
- **Custom allocators**: `mem_alloc()`, `mem_resize()`, `mem_free()` wrappers
- **Null checking**: Always validate pointers before use
- **Resource cleanup**: Explicit cleanup in compilation pipeline
- **Zero initialization**: Struct initialization with `{0}`

### Code Organization
- **Private functions first**: Mark internal functions with `internal` keyword
- **API functions last**: Public interface at bottom of files
- **Extensive comments**: Document function purposes and constraints
- **TODO markers**: Use `TODO(str)` macro for unimplemented features

### Platform Abstractions
- **OS detection**: `OS_WIN`, `OS_MAC`, `OS_LIN` macros
- **Color definitions**: Terminal color macros (`LRED`, `BOLD`, `RESET`)
- **Platform-specific handling**: Conditional compilation for different systems

### Switch Statement Style
- Comprehensive switch statements with all enum cases covered
- Return statements directly from cases
- Default cases that should never be reached use `UNREACHABLE()`

### String Handling
- Use `cstr` for read-only strings
- Manual string operations with bounds checking
- Length-aware string duplication with `string_dup()`

### Constants and Limits
- Define meaningful constants like `MAX_IDENTIFIER_LENGTH`, `EXTRA_NULL_TERMINATORS`
- Use static_assert for compile-time validation of type sizes
- Define project-specific constants in `defines.h`

## Development Notes

- The project uses C11 standard with GNU extensions
- Cross-platform support for Linux, macOS, and Windows
- Extensive compiler warnings enabled for code quality
- Parser implementation is partially complete and functional (see `src/fe/parser.c/.h`)
- Parser can be bypassed with `--lex` flag for lexer-only operation
- AST node types are defined but full parsing is still in development
- Version is defined as `RTVERSION "0.0.1"` in `defines.h`

## Current Implementation Status

### ✅ Completed Features
- **Lexer**: Complete tokenization with comprehensive token support
- **File I/O**: Robust file reading with error handling
- **CLI Interface**: Command-line argument parsing with multiple options
- **Build System**: Advanced Makefile with multiple build targets and tooling
- **Error Handling**: Comprehensive assertion and logging system
- **Memory Management**: Generic array/slice system with type safety
- **Testing**: Framework for running .vr test files
- **Cross-platform**: Support for Linux, macOS, and Windows

### 🚧 In Progress
- **Parser**: Basic AST structure implemented, parsing logic partially complete
- **Type System**: AST node definitions present, semantic analysis not implemented

### ❌ Not Implemented
- **Type Checker**: Semantic analysis stage
- **Code Generation**: Backend compilation to machine code
- **Standard Library**: Runtime library functions
- **Advanced Features**: Pattern matching, advanced memory management

## Test Coverage

The `test/` directory contains comprehensive test cases:
- `000_empty.vr` - Empty file handling
- `001_hello.vr` - Basic "Hello World" with imports
- `002_var.vr` - Variable declarations
- `003_comments.vr` - Comment parsing
- `004_func.vr` - Function definitions with parameters and return types
- `005_for_loop.vr` - For loop constructs
- `006_if.vr` - Conditional statements
- `007_while.vr` - While loop constructs
- `008_switch.vr` - Switch/case statements
- `009_struct.vr` - Struct definitions and field access
- `010_enum.vr` - Enumeration types
- `011_memory.vr` - Memory management constructs

## Build System Details

The Makefile has been significantly enhanced and now includes:

### New Build Targets
- `make help` - Display comprehensive help with examples
- `make test` - Run all .vr test files through the compiler
- `make test-zig` - Run Zig unit tests if present
- `make format` - Format code using clang-format (if available)
- `make check` - Run static analysis using cppcheck (if available)
- `make install` - Install binary to system (default: /usr/local/bin)
- `make uninstall` - Remove installed binary

### Build Improvements
- **Dependency tracking**: Automatic .d file generation for incremental builds
- **Color output**: Visual feedback during compilation with colored status messages
- **Better error handling**: Robust cross-platform commands and error checking
- **Performance options**: LTO (Link Time Optimization) in release builds
- **Enhanced debugging**: Address sanitizer and undefined behavior sanitizer in debug builds

### Usage Examples
```bash
# Build and run with arguments
make run ARGS="test/001_hello.vr --lex --timer"

# Install to custom location
make install INSTALL_PREFIX=/opt/rotate

# Debug build with sanitizers
make debug

# Static analysis and formatting
make check format
```