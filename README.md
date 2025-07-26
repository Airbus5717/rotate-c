
# Rotate Programming Language

A compiler for the Rotate programming language, written in C. Compiles source files
with `.vr` extension. This is version 0.0.1 - it's not ready for production use.

The compiler currently supports complete lexical analysis with partial parsing 
implementation. Don't expect it to compile anything useful yet.

## What works

* Complete lexical analysis with comprehensive tokenization
* Modular frontend/backend architecture  
* Cross-platform builds (Linux, macOS, Windows)
* Zig-based build system with fallback to GCC/Clang
* Test framework for .vr source files
* Address sanitizer and memory debugging support
* Type-safe generic arrays and slices using C macros

## Building

You need:
* Zig (recommended) or GCC/Clang
* Make 
* A working brain

```bash
git clone https://github.com/your-username/rotate-c.git
cd rotate-c
make
```

That's it. The binary ends up in `build/rotate`.

### Build targets

```bash
make          # release build (default)
make debug    # debug build with sanitizers
make safe     # extra safety checks
make help     # if you can't figure it out
```

## The Language

Syntax is straightforward. No surprises.

```rotate
io :: import "std/io"

fibonacci :: fn(n: i32) -> i32 {
    if n <= 1 {
        return n
    }
    return fibonacci(n-1) + fibonacci(n-2)
}

main :: fn() {
    io.println("Hello, Rotate!")
    number := 10
    result := fibonacci(number)
    io.printf("Fibonacci of %d is %d\n", number, result)
}
```

Features:
* Functions with proper type annotations
* Variables (:=) and constants (::) with type inference  
* Standard control flow (if/else, while, for, switch)
* Sensible integer types (i8, i16, i32, i64, u8, u16, u32, u64)
* Floating point (f32, f64)
* Structs and enums
* Explicit memory management with new/delete and defer
* Import system
* Pattern matching when we get around to it

## Directory Layout

```
src/fe/         frontend (lexer, parser)
src/utl/        utility functions  
src/include/    headers
src/            main compiler logic
test/           test .vr files
build/          build artifacts
```

## How it works

Traditional multi-pass compiler:

1. Read file
2. Lex into tokens
3. Parse into AST (incomplete)
4. Type check (not implemented)  
5. Generate code (not implemented)

The lexer is complete. Parser is stubbed out. Everything else is TODO.

## Usage

```bash
./build/rotate file.vr [--lex] [--log] [--timer] [--version]
```

* `--lex` - just tokenize, don't parse
* `--log` - dump debug info to output.org  
* `--timer` - show timing
* `--version` - print version and exit

Examples:
```bash
./build/rotate test/001_hello.vr --lex
make run ARGS="test/001_hello.vr --timer"
```

## Testing

```bash
make test      # run all .vr test files  
make test-zig  # run zig unit tests if present
```

## Development

```bash
make format    # format code (needs clang-format)
make check     # static analysis (needs cppcheck)  
make clean     # remove build files
```

Use `make debug` for address sanitizer builds. Use `make safe` for extra checks.

## Implementation Notes

### Arrays and Slices

Type-safe generic arrays using C macros. Not as pretty as C++ templates but it works:

```c
generate_array_type(int);

Array_int numbers = array_make(int, 10);
array_push(numbers, 42);
array_for_each(numbers, num) {
    printf("%d\n", *num);
}
array_free(numbers);
```

### Error Handling

Simple assertion macros and status codes:

```c
ASSERT(ptr != NULL, "Pointer cannot be null");
ASSERT_RET_FAIL(size > 0, "Size must be positive");  
TODO("Implement this someday");
```

## Status

What's done:
* Lexer 
* File I/O
* Command line interface
* Test framework

What's not done:
* Parser (started)
* Type checker  
* Code generation
* Standard library
* Everything else

This is version 0.0.1. Don't use it for anything important.

## License

See LICENSE file.
