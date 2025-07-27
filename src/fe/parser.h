#pragma once

#include "lexer.h"
#include "type.h"

typedef enum
{
    PE_UNKNOWN,
    PE_OUT_OF_MEMORY,
    PE_UNEXPECTED_TOKEN,
    PE_EXPECTED_IDENTIFIER,
    PE_EXPECTED_EXPRESSION,
    PE_EXPECTED_TYPE,
    PE_EXPECTED_STATEMENT,
    PE_EXPECTED_DECLARATION,
    PE_UNMATCHED_PAREN,
    PE_UNMATCHED_BRACE,
    PE_UNMATCHED_BRACKET,
    PE_INVALID_ASSIGNMENT_TARGET,
    PE_DUPLICATE_PARAMETER,
    PE_EMPTY_PARAMETER_LIST,
    PE_INVALID_FUNCTION_SIGNATURE,
    PE_INVALID_STRUCT_FIELD,
    PE_INVALID_ENUM_MEMBER,
} ParseErr;

typedef struct Parser
{
    Lexer *lexer;
    uint index;
    AstProgram *ast;
    ParseErr error;
    uint error_line;
    uint error_col;
} Parser;

Parser parser_init(Lexer *);
u8 parser_parse(Parser *);
void parser_deinit(Parser *);

// Error handling functions
cstr parser_err_msg(const ParseErr error);
cstr parser_err_advice(const ParseErr error);
u8 parser_report_error(Parser *p);

// AST creation functions
AstProgram *ast_program_create(void);
AstDecl *ast_decl_create(AstNodeType kind, Token token);
AstStmt *ast_stmt_create(AstNodeType kind, Token token);
AstExpr *ast_expr_create(AstNodeType kind, Token token);
AstType *ast_type_create(AstNodeType kind, Token token);

// AST destruction functions
void ast_program_free(AstProgram *program);
void ast_decl_free(AstDecl *decl);
void ast_stmt_free(AstStmt *stmt);
void ast_expr_free(AstExpr *expr);
void ast_type_free(AstType *type);

// internal methods are in parser.c
