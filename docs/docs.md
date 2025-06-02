

# Rotate Programming Language Documentation

This document provides an up-to-date overview of the Rotate programming language, outlining its core syntax and language features with consistent examples.

---

## Hello World

The following example demonstrates how to import the IO standard library and print "Hello World" to the console:

```go
io :: import "std/io"

main :: fn() {
    io.println("Hello World")
}
```

---

## Comments

Rotate supports both single-line and multi-line comments:

```go
// This is a single-line comment

/* 
   This is a multi-line comment.
   In Rotate, a multi-line comment does not require an explicit ending delimiter—
   the end-of-file is sufficient.
*/

main :: fn() {}
```

---

## Variables

Variables are declared using `:=` for inferred variables and `::` for inferred constants. You can also provide explicit type information when needed:

```go
io :: import "std/io"

main :: fn() {
    a := 1         // Inferred variable
    b :: 2         // Inferred constant
    c :int = 3     // Integer variable with explicit type

    io.print_i(a)
    io.print_i(b)
    io.print_i(c)
    io.println("") // Print newline
}
```

---

## Control Structures

### If-Else Statements

Rotate uses an `if` statement with an optional `else` branch for conditional execution:

```go
io :: import "std/io"

main :: fn() {
    x := 1
    if x == 1 {
        io.println("x is 1")
    } else {
        io.println("x is not 1")
    }
}
```

### Loops

#### For Loop

Rotate supports a `for` loop to iterate over a range:

```go
main :: fn() {
    for i in 0..3 {
        // Loop body: i takes on values 0, 1, 2
    }
}
```

#### While Loop

The `while` loop executes as long as the given condition is true:

```go
main :: fn() {
    condition := true
    while condition {
        // Loop body
        condition = false  // Terminate loop after one iteration
    }
}
```

### Switch Statement

The `switch` construct facilitates multi-way branching:

```go
main :: fn() {
    value :: 1
    switch value {
        1: {
            io.println("Value is 1")
        }
        2: {
            io.println("Value is 2")
        }
        else: {
            io.println("Value is unknown")
        }
    }
}
```

---

## Structs

Structures are defined to create custom composite data types:

```go
Token :: struct {
    id: int,
    data: [3]int,
}

main :: fn() {
    t := Token{ 1, [10, 20, 30] }
    t.id = 2
    t.data[1] = 99
    // Use t as needed
}
```

---

## Functions

Functions are declared with the `fn` keyword and can include type declarations for parameters and return values:

```go
io :: import "std/io"

add :: fn(x: int, y: int) int {
    return x + y
}

main :: fn() {
    result := add(5, 7)
    io.print_i(result)  // Output should be 12
    io.println("")
}
```

---

## Enums

Enums define a set of constant identifiers:

```go
TknType :: enum {
    Id,
    Number,
    Float,
    String,
    Char,
    Invalid,
}

main :: fn() {
    currentType :: TknType::Id
    // Further processing based on currentType
}
```

---

## Memory Management

Rotate uses C-like memory management with modern features such as `defer` to ensure cleanup:

```go
io :: import "std/io"
os :: import "std/os"

main :: fn() {
    buffer := new [3]char
    defer delete buffer

    nums := new [100]int
    defer delete nums

    if nums == nil {
        io.println("Allocation failed")
        os.exit(1)
    }
    
    // Process buffer and nums as needed
}
```

---

This documentation reflects the latest syntax and features of the Rotate programming language. Use these examples as a guide to writing programs in Rotate and refer back to this document as the language evolves.
