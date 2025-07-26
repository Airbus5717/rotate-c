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

Build outputs go to `build/` directory with the main executable at `build/main`.

## Testing

- Test files are located in `test/` directory with `.vr` extension
- Run tests with the compiler: `./build/main test/001_hello.vr`
- There's a Zig test file at `src/test.zig` - run with `zig test src/test.zig`

## Compiler Usage

```bash
./build/main <source_file.vr> [options]
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
3. **Parsing** (`ST_PARSER`) - Convert tokens to AST (currently commented out/incomplete)
4. **Type Checking** (`ST_TCHECKER`) - Not yet implemented
5. **Logging** (`ST_LOGGER`) - Optional debug output generation

### Key Components

- **Frontend (`src/fe/`)**:
  - `lexer.c/.h` - Tokenization engine with comprehensive token types
  - `parser.c/.h` - AST generation (work in progress)
  - `token.c/.h` - Token definitions and utilities
  - `type.c/.h` - Type system definitions

- **Core Systems (`src/`)**:
  - `main.c` - Entry point and command line parsing
  - `compile.c` - Main compilation orchestration
  - `file.c` - File I/O utilities
  - `log.c` - Logging and debug output

- **Utilities (`src/utl/`)**:
  - `common.c` - Shared utility functions

- **Headers (`src/include/`)**:
  - `defines.h` - Core type definitions, macros, platform detection
  - `common.h` - Shared function declarations
  - `arraylist.h` - Generic dynamic array and slice system with macro-based implementation
  - `mem.h` - Memory management utilities

### Token System

The lexer supports a comprehensive set of tokens defined in `src/fe/token.h`, including:
- Keywords (fn, let, if, else, while, for, struct, enum, etc.)
- Literals (integers, floats, strings, chars, booleans)
- Operators (arithmetic, logical, bitwise, comparison)
- Delimiters (parentheses, brackets, braces)

### Language Features

Based on the documentation and test files, Rotate supports:
- Functions with type annotations
- Variables (`:=`) and constants (`::`) with type inference
- Control flow (if/else, loops, switch)
- Structs and enums
- Memory management with `new`/`delete` and `defer`
- Import system for standard libraries

### Error Handling

The codebase uses extensive assertion macros defined in `defines.h`:
- `ASSERT(expr, msg)` - Runtime assertions
- `ASSERT_RET_FAIL(expr, msg)` - Assertions that return failure status
- `TODO(str)` - Mark unimplemented features
- `UNREACHABLE()` - Mark code paths that should never execute

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
- **Extensive typedef usage**: `u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64`
- **Float types**: `f32`, `f64`, `f128` (platform-dependent)
- **Size types**: `usize`, `isize` (64-bit aligned)
- **Convenience types**: `cstr` for const char*, `uint` for u32

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
- Parser implementation is currently incomplete (commented out in `compile.c:42-48`)
- Version is defined as `RTVERSION "0.0.1"` in `defines.h`

## Project Milestones

- The lexer is complete, and single file read