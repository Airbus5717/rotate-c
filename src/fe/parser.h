#pragma once

#include "lexer.h"
#include "type.h"

typedef struct Parser
{
    Lexer *lexer;
    uint index;
    AstProgram *ast;
} Parser;

Parser parser_init(Lexer *);
u8 parser_parse(Parser *);
void parser_deinit(Parser *);

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
