#pragma once

#include "lexer.hpp"
#include "token.hpp"

namespace rotate
{
//! Keep code as simple as possible
//! for scope level bases
//
//

// types
struct Type;
struct ArrayType;
struct LiteralType;

// Expressions
struct Expr;
struct LitExpr;
struct BinaryExpr;
struct UnaryExpr;

// Ast and Symbol table
struct Symbol;
struct SymbolTable;
struct Ast;
struct AstImport;
struct AstGlVar;
struct AstStruct;
struct AstEnum;
struct AstFn;

enum class PrsErr : u16
{
    Unknown = 0,
    GlobalScopeNotAllowed,
    OpenParents,
    CloseParent,
    StringExpect,
    CharExpect,
    SemicolonExpect,
};

enum class TypeType
{
    undecided,
    _boolean,
    _integer,
    _float,
    _chr,
    _array,
    _struct,
    _enum,
};

struct LiteralType
{
};

struct ArrayType
{
    Type *type;
    // NOTE(5717): NULL means heap array
    Uint *length;
};

struct Type
{
    TypeType type;
    union UType {
        LiteralType lit;
        ArrayType arr;
    };
};
/*
 * Expressions
 */
enum class LiteralExprType
{
    // TODO:
    _integer,
    _float,
    _array,
};

enum class BinaryOpType
{
    // TODO:
    add,  // +
    sub,  // -
    mult, // *
    div,  // '/'
    eqleql,
    greater,
    greql,
    less,
    leseql,
};

enum class UnaryOpType
{
    logical_negate,   // '!'
    numerical_negate, // -
    new_opr,          // new (only allowed as expressions)
    delete_opr,       // delete (only allowed as stmts)
};

struct LitExpr
{
    LiteralExprType type;
    Uint val_idx;
};

struct UnaryExpr
{
    UnaryOpType op;
    Expr *expr;
};

struct BinaryExpr
{
    BinaryOpType op;
    Expr *left, *right;
};

struct Expr
{
    // TODO: Make a union
    LitExpr literal;
    UnaryExpr unary;
    BinaryExpr binary;
};

/*
 * Symbols section
 *  */
struct Symbol
{
    Uint id_idx;
};

// NOTE(5717): database of symbols
struct SymbolTable
{
    std::vector<Symbol> symbols;
};

/*
 * Abstract Syntax tree
 */
struct Ast
{
    SymbolTable smbl_table;
    std::vector<AstImport> imports;
    std::vector<AstGlVar> gl_variables;
    std::vector<AstStruct> structs;
    std::vector<AstEnum> enums;
    std::vector<AstFn> functions;
};

struct AstImport
{

    // 0  1  2     34    56
    // io :: import("std");
    // ^^id          ^^^value
    Uint id_idx;
    Uint val_idx;

    AstImport(Uint val) : id_idx(val - 4), val_idx(val)
    {
    }

    ~AstImport() = default;
};

struct AstGlVar
{
    // NOTE(5717): GL Variables must be const
    // and at comptime known their value
    // id, (comptime)value, type
    Uint id_idx;
    Type type;
};

struct AstStruct
{
    // id, params(),
};

struct AstEnum
{
};

struct AstBlock
{
};

struct AstFn
{
    AstBlock block;
    SymbolTable sub_table;
};

class Parser
{
    // not owned (weak ptr)
    file_t *file;
    //
    Ast ast;
    std::vector<Token> *tokens;
    PrsErr error;
    Uint idx;

    u8 parse_director();
    // global stmts
    u8 parse_import();
    u8 parse_function();
    u8 parse_gl_var();
    u8 parse_struct();
    u8 parse_enum();
    // non global
    u8 parse_block();

    // expressions
    u8 parse_expr();
    u8 parse_bin_expr();
    u8 parse_unary_expr();
    u8 parse_grouping_expr();
    u8 parse_literal();

    // utils
    Token current() const;
    Token past() const;
    Token peek() const;
    void advance();

    //
    u8 parse_error_expect_token(TknType);
    PrsErr convert_tkn_type_to_parse_error(TknType);
    u8 parser_error(PrsErr);
    const char *parser_error_msg(PrsErr);
    const char *parser_error_advice(PrsErr);
    u8 parse_error_use_global_err();

    // Parser exports
    std::vector<AstImport> imports;
    std::vector<AstFn> functions;
    std::vector<AstEnum> enums;
    std::vector<AstStruct> structs;
    std::vector<AstGlVar> glVars;

  public:
    Parser(file_t *, Lexer *);
    ~Parser();
    u8 parse_lexer();
}; // Parser

} // namespace rotate