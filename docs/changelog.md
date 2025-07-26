# Rotate Compiler Changelog

## [Update] - 2025-07-26
- first update using AI 

### Added
- **Test Suite Enhancements**: Created comprehensive test files based on language documentation
  - `test/005_for_loop.vr` - For loop with range syntax (`for i in 0..3`)
  - `test/007_while.vr` - While loop implementation
  - `test/008_switch.vr` - Switch statement with multiple cases and else branch
  - `test/009_struct.vr` - Struct definition and usage example
  - `test/010_enum.vr` - Enum definition and member access
  - `test/011_memory.vr` - Memory management with `new`/`delete`/`defer`

- **Parser Implementation**: Comprehensive AST-based parser system
  - Full AST node types for expressions, statements, declarations, and types
  - Parser functions for all language constructs (functions, variables, structs, enums, imports)
  - Expression parsing with operator precedence
  - Statement parsing for control flow (if, while, for, return, defer)
  - Memory management for AST nodes with proper cleanup functions

- **Parser Integration**: Connected parser to compilation pipeline
  - Enabled parser stage in `src/compile.c` (previously commented out)
  - Added parser initialization, parsing, and cleanup to compilation flow
  - Updated logging system to include parser information

### Changed
- **Build System**: Updated Makefile compiler configuration
  - Changed default compiler from `zig cc` to `clang`
  - Reorganized build target dependencies for better structure

- **Test Corrections**: Fixed syntax errors in existing test files to match documentation
  - `test/002_var.vr`: Corrected function declaration syntax (`main :: fn()` instead of `fn main()`)
  - `test/002_var.vr`: Updated variable names and values to match documentation examples
  - `test/004_func.vr`: Changed `ret` keyword to `return` for proper return statement syntax
  - `test/004_func.vr`: Updated function example to use documented values (5, 7 → 12)
  - `test/006_if.vr`: Removed parentheses around if condition (Rotate style)
  - `test/006_if.vr`: Added missing `io.` prefix to `println` calls

- **Array System Enhancements**: Extended generic array functionality
  - Added new array generation macros and type definitions
  - Improved array bounds checking and safety mechanisms

- **Type System**: Enhanced type definitions and handling
  - Extended type system with additional AST node types
  - Improved type checking infrastructure preparation

- **Logging System**: Enhanced debug output capabilities
  - Updated logging functions to handle parser information
  - Improved error reporting and debug output formatting

### Technical Details

#### Parser Architecture
- **AST Node Types**: Complete set of node types for all language constructs
  - Declarations: Functions, variables, structs, enums, imports
  - Statements: Expressions, blocks, if/else, loops, return, defer
  - Expressions: Binary operations, unary operations, function calls, literals
  - Types: Basic types, arrays, function signatures

- **Memory Management**: Comprehensive cleanup system
  - Individual free functions for each AST node type
  - Recursive cleanup of nested structures
  - Proper array and pointer management

- **Error Handling**: Robust parser error reporting
  - Descriptive error messages for syntax violations
  - Early return patterns for error propagation
  - Safe cleanup on parse failures

#### Test Coverage
The new test suite covers all documented language features:
- **Basic syntax**: Hello world, comments, variables
- **Functions**: Declaration, parameters, return types, calls
- **Control flow**: If/else statements, while loops, for loops, switch statements
- **Data structures**: Structs with fields, enums with members
- **Memory management**: Dynamic allocation with `new`, cleanup with `defer`
- **Module system**: Import statements with aliasing

All tests now conform to the official language specification documented in `docs/docs.md`.
