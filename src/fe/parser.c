#include "parser.h"
#include "token.h"
#include "type.h"
#include "../include/mem.h"
#include "../include/arraylist.h"
/*
 *
 * Internal private functions definitions
 *
 */
internal u8 parse_director(Parser *);
internal AstDecl *parse_import(Parser *);
internal AstDecl *parse_function(Parser *);
internal AstDecl *parse_struct(Parser *);
internal AstDecl *parse_enum(Parser *);
internal AstDecl *parse_variable(Parser *);
internal AstStmt *parse_statement(Parser *);
internal AstStmt *parse_block(Parser *);
internal AstStmt *parse_if_statement(Parser *);
internal AstStmt *parse_while_statement(Parser *);
internal AstStmt *parse_for_statement(Parser *);
internal AstStmt *parse_return_statement(Parser *);
internal AstExpr *parse_expression(Parser *);
internal AstExpr *parse_assignment(Parser *);
internal AstExpr *parse_logical_or(Parser *);
internal AstExpr *parse_logical_and(Parser *);
internal AstExpr *parse_equality(Parser *);
internal AstExpr *parse_comparison(Parser *);
internal AstExpr *parse_term(Parser *);
internal AstExpr *parse_factor(Parser *);
internal AstExpr *parse_unary(Parser *);
internal AstExpr *parse_call(Parser *);
internal AstExpr *parse_primary(Parser *);
internal AstType *parse_type(Parser *);

// NOTE(5717): useful parser utils
inline internal Token
current(Parser *p)
{
    if (p->index >= array_count(p->lexer->tokens)) {
        Token eof = {0};
        eof.type = Tkn_EOT;
        return eof;
    }
    return array_at(p->lexer->tokens, (p->index));
}

inline internal Token
next(Parser *p)
{
    if (p->index + 1 >= array_count(p->lexer->tokens)) {
        Token eof = {0};
        eof.type = Tkn_EOT;
        return eof;
    }
    return array_at(p->lexer->tokens, (p->index + 1));
}

inline internal Token
previous(Parser *p)
{
    if (p->index == 0) {
        Token eof = {0};
        eof.type = Tkn_EOT;
        return eof;
    }
    return array_at(p->lexer->tokens, (p->index - 1));
}

inline internal void
advance(Parser *p)
{
    if (p->index < array_count(p->lexer->tokens)) {
        p->index++;
    }
}

inline internal bool
check(Parser *p, TknType type)
{
    return current(p).type == type;
}

inline internal bool
match(Parser *p, TknType type)
{
    if (check(p, type)) {
        advance(p);
        return true;
    }
    return false;
}

inline internal u8
expect_n_consume(Parser *p, TknType t)
{
    Token tkn = current(p);
    if (tkn.type == t)
    {
        advance(p);
        return SUCCESS;
    }
    return FAILURE;
}

inline internal void
skip_terminators(Parser *p)
{
    while (match(p, Tkn_Terminator)) {
        // Skip newlines and semicolons
    }
}
/*
 *
 * AST Creation Functions
 *
 */
AstProgram *
ast_program_create(void)
{
    AstProgram *program = mem_alloc(sizeof(AstProgram));
    program->declarations = array_make(AstDeclPtr, 8);
    return program;
}

AstDecl *
ast_decl_create(AstNodeType kind, Token token)
{
    AstDecl *decl = mem_alloc(sizeof(AstDecl));
    decl->kind = kind;
    decl->token = token;
    return decl;
}

AstStmt *
ast_stmt_create(AstNodeType kind, Token token)
{
    AstStmt *stmt = mem_alloc(sizeof(AstStmt));
    stmt->kind = kind;
    stmt->token = token;
    return stmt;
}

AstExpr *
ast_expr_create(AstNodeType kind, Token token)
{
    AstExpr *expr = mem_alloc(sizeof(AstExpr));
    expr->kind = kind;
    expr->token = token;
    expr->type = nullptr;
    return expr;
}

AstType *
ast_type_create(AstNodeType kind, Token token)
{
    AstType *type = mem_alloc(sizeof(AstType));
    type->kind = kind;
    type->token = token;
    return type;
}

/*
 *
 * AST Destruction Functions
 *
 */
void
ast_expr_free(AstExpr *expr)
{
    if (!expr) return;
    
    switch (expr->kind) {
        case AST_EXPR_BINARY:
            ast_expr_free(expr->binary.left);
            ast_expr_free(expr->binary.right);
            break;
        case AST_EXPR_UNARY:
            ast_expr_free(expr->unary.operand);
            break;
        case AST_EXPR_CALL:
            ast_expr_free(expr->call.callee);
            for (usize i = 0; i < array_count(expr->call.arguments); i++) {
                ast_expr_free(array_at(expr->call.arguments, i));
            }
            array_free(expr->call.arguments);
            break;
        case AST_EXPR_MEMBER:
            ast_expr_free(expr->member.object);
            break;
        case AST_EXPR_ASSIGN:
            ast_expr_free(expr->assign.target);
            ast_expr_free(expr->assign.value);
            break;
        default:
            break;
    }
    mem_free(expr);
}

void
ast_stmt_free(AstStmt *stmt)
{
    if (!stmt) return;
    
    switch (stmt->kind) {
        case AST_STMT_EXPR:
            ast_expr_free(stmt->expr.expression);
            break;
        case AST_STMT_IF:
            ast_expr_free(stmt->if_stmt.condition);
            ast_stmt_free(stmt->if_stmt.then_stmt);
            ast_stmt_free(stmt->if_stmt.else_stmt);
            break;
        case AST_STMT_WHILE:
            ast_expr_free(stmt->while_stmt.condition);
            ast_stmt_free(stmt->while_stmt.body);
            break;
        case AST_STMT_FOR:
            ast_stmt_free(stmt->for_stmt.init);
            ast_expr_free(stmt->for_stmt.condition);
            ast_stmt_free(stmt->for_stmt.update);
            ast_stmt_free(stmt->for_stmt.body);
            break;
        case AST_STMT_RETURN:
            ast_expr_free(stmt->return_stmt.value);
            break;
        case AST_STMT_DEFER:
            ast_stmt_free(stmt->defer_stmt.statement);
            break;
        case AST_STMT_BLOCK:
            for (usize i = 0; i < array_count(stmt->block.statements); i++) {
                ast_stmt_free(array_at(stmt->block.statements, i));
            }
            array_free(stmt->block.statements);
            break;
        default:
            break;
    }
    mem_free(stmt);
}

void
ast_decl_free(AstDecl *decl)
{
    if (!decl) return;
    
    switch (decl->kind) {
        case AST_DECL_FUNCTION:
            for (usize i = 0; i < array_count(decl->function.parameters); i++) {
                ast_decl_free(array_at(decl->function.parameters, i));
            }
            array_free(decl->function.parameters);
            ast_type_free(decl->function.return_type);
            ast_stmt_free(decl->function.body);
            break;
        case AST_DECL_VARIABLE:
            ast_type_free(decl->variable.type);
            ast_expr_free(decl->variable.initializer);
            break;
        case AST_DECL_STRUCT:
            for (usize i = 0; i < array_count(decl->struct_decl.fields); i++) {
                ast_decl_free(array_at(decl->struct_decl.fields, i));
            }
            array_free(decl->struct_decl.fields);
            break;
        case AST_DECL_ENUM:
            for (usize i = 0; i < array_count(decl->enum_decl.members); i++) {
                ast_decl_free(array_at(decl->enum_decl.members, i));
            }
            array_free(decl->enum_decl.members);
            break;
        default:
            break;
    }
    mem_free(decl);
}

void
ast_type_free(AstType *type)
{
    if (!type) return;
    
    switch (type->kind) {
        case AST_TYPE_ARRAY:
            ast_type_free(type->array.element_type);
            ast_expr_free(type->array.size);
            break;
        case AST_TYPE_FUNCTION:
            for (usize i = 0; i < array_count(type->function.param_types); i++) {
                ast_type_free(array_at(type->function.param_types, i));
            }
            array_free(type->function.param_types);
            ast_type_free(type->function.return_type);
            break;
        default:
            break;
    }
    mem_free(type);
}

void
ast_program_free(AstProgram *program)
{
    if (!program) return;
    
    for (usize i = 0; i < array_count(program->declarations); i++) {
        ast_decl_free(array_at(program->declarations, i));
    }
    array_free(program->declarations);
    mem_free(program);
}

/*
 *
 * Public functions
 *
 */
Parser
parser_init(Lexer *l)
{
    Parser parser = {0};
    parser.lexer = l;
    parser.index = 0;
    parser.ast = ast_program_create();
    return parser;
}

u8
parser_parse(Parser *p)
{
    return parse_director(p);
}

void
parser_deinit(Parser *p)
{
    if (p->ast) {
        ast_program_free(p->ast);
        p->ast = nullptr;
    }
}

/*
 *
 * internal functions
 */

u8
parse_director(Parser *p)
{
    skip_terminators(p);
    
    while (!check(p, Tkn_EOT)) {
        AstDecl *decl = nullptr;
        
        // Check if this is a variable or constant declaration: identifier :: 
        if (check(p, Tkn_Identifier) && next(p).type == Tkn_Colon) {
            Token name_token = current(p);
            advance(p); // consume identifier
            advance(p); // consume :
            
            if (match(p, Tkn_Colon)) {
                // :: - constant or import or function
                if (check(p, Tkn_ImportKeyword)) {
                    // Rewind to identifier and parse as import
                    p->index -= 3; // Back to identifier
                    decl = parse_import(p);
                } else if (check(p, Tkn_FnKeyword)) {
                    // Rewind to identifier and parse as function
                    p->index -= 3; // Back to identifier
                    decl = parse_function(p);
                } else {
                    // Parse as constant
                    p->index -= 2;
                    decl = parse_variable(p);
                }
            } else {
                // : - variable declaration
                p->index -= 2;
                decl = parse_variable(p);
            }
        } else {
            switch (current(p).type) {
                case Tkn_ImportKeyword: 
                    decl = parse_import(p); 
                    break;
                case Tkn_FnKeyword: 
                    decl = parse_function(p); 
                    break;
                case Tkn_StructKeyword: 
                    decl = parse_struct(p); 
                    break;
                case Tkn_EnumKeyword: 
                    decl = parse_enum(p); 
                    break;
                case Tkn_LetKeyword: 
                    decl = parse_variable(p); 
                    break;
                case Tkn_EOT: 
                    return SUCCESS;
                default:
                    log_error("Unexpected token in global scope");
                    return FAILURE;
            }
        }
        
        if (!decl) {
            return FAILURE;
        }
        
        array_push(p->ast->declarations, decl);
        skip_terminators(p);
    }
    
    return SUCCESS;
}

AstDecl *
parse_import(Parser *p)
{
    Token import_token = current(p);
    AstDecl *import_decl = ast_decl_create(AST_DECL_IMPORT, import_token);
    
    // Parse: alias :: import "module/path"
    if (check(p, Tkn_Identifier)) {
        import_decl->import.alias = current(p);
        advance(p);
        
        if (!match(p, Tkn_Colon) || !match(p, Tkn_Colon)) {
            log_error("Expected '::' after import alias");
            ast_decl_free(import_decl);
            return nullptr;
        }
    }
    
    if (!match(p, Tkn_ImportKeyword)) {
        log_error("Expected 'import' keyword");
        ast_decl_free(import_decl);
        return nullptr;
    }
    
    if (!check(p, Tkn_StringLiteral)) {
        log_error("Expected string literal for import path");
        ast_decl_free(import_decl);
        return nullptr;
    }
    
    import_decl->import.module_path = current(p);
    advance(p);
    
    return import_decl;
}

AstDecl *
parse_function(Parser *p)
{
    Token func_token = current(p);
    AstDecl *func_decl = ast_decl_create(AST_DECL_FUNCTION, func_token);
    func_decl->function.parameters = array_make(AstDeclPtr, 4);
    
    // Parse: name :: fn(params) return_type { body }
    // or: fn name(params) return_type { body }
    
    if (check(p, Tkn_Identifier)) {
        func_decl->function.name = current(p);
        advance(p);
        
        if (!match(p, Tkn_Colon) || !match(p, Tkn_Colon)) {
            log_error("Expected '::' after function name");
            ast_decl_free(func_decl);
            return nullptr;
        }
    }
    
    if (!match(p, Tkn_FnKeyword)) {
        log_error("Expected 'fn' keyword");
        ast_decl_free(func_decl);
        return nullptr;
    }
    
    // If we haven't parsed a name yet, parse it now (function.name starts with type 0, which is Tkn_Identifier)
    // Check if we parsed the name in the first branch by seeing if we have a valid identifier at the right position
    if (func_decl->function.name.index == 0 && func_decl->function.name.length == 0) {
        if (!check(p, Tkn_Identifier)) {
            log_error("Expected function name");
            ast_decl_free(func_decl);
            return nullptr;
        }
        func_decl->function.name = current(p);
        advance(p);
    }
    
    // Parse parameters
    if (!match(p, Tkn_OpenParen)) {
        log_error("Expected '(' after function name");
        ast_decl_free(func_decl);
        return nullptr;
    }
    
    while (!check(p, Tkn_CloseParen) && !check(p, Tkn_EOT)) {
        if (!check(p, Tkn_Identifier)) {
            log_error("Expected parameter name");
            ast_decl_free(func_decl);
            return nullptr;
        }
        
        AstDecl *param = ast_decl_create(AST_DECL_VARIABLE, current(p));
        param->variable.name = current(p);
        advance(p);
        
        if (match(p, Tkn_Colon)) {
            param->variable.type = parse_type(p);
            if (!param->variable.type) {
                ast_decl_free(param);
                ast_decl_free(func_decl);
                return nullptr;
            }
        }
        
        array_push(func_decl->function.parameters, param);
        
        if (!match(p, Tkn_Comma)) {
            break;
        }
    }
    
    if (!match(p, Tkn_CloseParen)) {
        log_error("Expected ')' after parameters");
        ast_decl_free(func_decl);
        return nullptr;
    }
    
    // Parse return type (optional)
    if (!check(p, Tkn_OpenCurly)) {
        func_decl->function.return_type = parse_type(p);
        if (!func_decl->function.return_type) {
            ast_decl_free(func_decl);
            return nullptr;
        }
    }
    
    // Parse body
    func_decl->function.body = parse_block(p);
    if (!func_decl->function.body) {
        ast_decl_free(func_decl);
        return nullptr;
    }
    
    return func_decl;
}

AstDecl *
parse_variable(Parser *p)
{
    Token var_token = current(p);
    AstDecl *var_decl = ast_decl_create(AST_DECL_VARIABLE, var_token);
    var_decl->variable.is_constant = false;
    
    bool has_let = false;
    if (match(p, Tkn_LetKeyword)) {
        has_let = true;
    }
    
    if (!check(p, Tkn_Identifier)) {
        log_error("Expected variable name");
        ast_decl_free(var_decl);
        return nullptr;
    }
    
    var_decl->variable.name = current(p);
    advance(p);
    
    if (match(p, Tkn_Colon)) {
        if (match(p, Tkn_Colon)) {
            // :: - constant declaration
            var_decl->variable.is_constant = true;
        } else {
            // : - typed variable
            var_decl->variable.type = parse_type(p);
            if (!var_decl->variable.type) {
                ast_decl_free(var_decl);
                return nullptr;
            }
            
            if (!match(p, Tkn_Equal)) {
                log_error("Expected '=' after variable type");
                ast_decl_free(var_decl);
                return nullptr;
            }
        }
    } else if (!has_let) {
        // Shorthand declaration: x := value
        if (!match(p, Tkn_Colon) || !match(p, Tkn_Equal)) {
            log_error("Expected ':=' for variable declaration");
            ast_decl_free(var_decl);
            return nullptr;
        }
    } else {
        log_error("Expected ':' after variable name");
        ast_decl_free(var_decl);
        return nullptr;
    }
    
    // Parse initializer
    var_decl->variable.initializer = parse_expression(p);
    if (!var_decl->variable.initializer) {
        ast_decl_free(var_decl);
        return nullptr;
    }
    
    return var_decl;
}

AstDecl *
parse_struct(Parser *p)
{
    Token struct_token = current(p);
    advance(p); // consume 'struct'
    
    AstDecl *struct_decl = ast_decl_create(AST_DECL_STRUCT, struct_token);
    struct_decl->struct_decl.fields = array_make(AstDeclPtr, 8);
    
    if (!check(p, Tkn_Identifier)) {
        log_error("Expected struct name");
        ast_decl_free(struct_decl);
        return nullptr;
    }
    
    struct_decl->struct_decl.name = current(p);
    advance(p);
    
    if (!match(p, Tkn_OpenCurly)) {
        log_error("Expected '{' after struct name");
        ast_decl_free(struct_decl);
        return nullptr;
    }
    
    while (!check(p, Tkn_CloseCurly) && !check(p, Tkn_EOT)) {
        skip_terminators(p);
        
        if (!check(p, Tkn_Identifier)) {
            log_error("Expected field name");
            ast_decl_free(struct_decl);
            return nullptr;
        }
        
        AstDecl *field = ast_decl_create(AST_DECL_VARIABLE, current(p));
        field->variable.name = current(p);
        advance(p);
        
        if (!match(p, Tkn_Colon)) {
            log_error("Expected ':' after field name");
            ast_decl_free(field);
            ast_decl_free(struct_decl);
            return nullptr;
        }
        
        field->variable.type = parse_type(p);
        if (!field->variable.type) {
            ast_decl_free(field);
            ast_decl_free(struct_decl);
            return nullptr;
        }
        
        array_push(struct_decl->struct_decl.fields, field);
        skip_terminators(p);
    }
    
    if (!match(p, Tkn_CloseCurly)) {
        log_error("Expected '}' after struct fields");
        ast_decl_free(struct_decl);
        return nullptr;
    }
    
    return struct_decl;
}

AstDecl *
parse_enum(Parser *p)
{
    Token enum_token = current(p);
    advance(p); // consume 'enum'
    
    AstDecl *enum_decl = ast_decl_create(AST_DECL_ENUM, enum_token);
    enum_decl->enum_decl.members = array_make(AstDeclPtr, 8);
    
    if (!check(p, Tkn_Identifier)) {
        log_error("Expected enum name");
        ast_decl_free(enum_decl);
        return nullptr;
    }
    
    enum_decl->enum_decl.name = current(p);
    advance(p);
    
    if (!match(p, Tkn_OpenCurly)) {
        log_error("Expected '{' after enum name");
        ast_decl_free(enum_decl);
        return nullptr;
    }
    
    while (!check(p, Tkn_CloseCurly) && !check(p, Tkn_EOT)) {
        skip_terminators(p);
        
        if (!check(p, Tkn_Identifier)) {
            log_error("Expected enum member name");
            ast_decl_free(enum_decl);
            return nullptr;
        }
        
        AstDecl *member = ast_decl_create(AST_DECL_VARIABLE, current(p));
        member->variable.name = current(p);
        advance(p);
        
        array_push(enum_decl->enum_decl.members, member);
        
        if (!match(p, Tkn_Comma)) {
            break;
        }
        skip_terminators(p);
    }
    
    if (!match(p, Tkn_CloseCurly)) {
        log_error("Expected '}' after enum members");
        ast_decl_free(enum_decl);
        return nullptr;
    }
    
    return enum_decl;
}

// Statement parsing functions
AstStmt *
parse_statement(Parser *p)
{
    switch (current(p).type) {
        case Tkn_IfKeyword:
            return parse_if_statement(p);
        case Tkn_WhileKeyword:
            return parse_while_statement(p);
        case Tkn_ForKeyword:
            return parse_for_statement(p);
        case Tkn_RetKeyword:
            return parse_return_statement(p);
        case Tkn_OpenCurly:
            return parse_block(p);
        case Tkn_LetKeyword: {
            AstDecl *var_decl = parse_variable(p);
            if (!var_decl) return nullptr;
            
            AstStmt *stmt = ast_stmt_create(AST_STMT_EXPR, var_decl->token);
            stmt->expr.expression = ast_expr_create(AST_EXPR_LITERAL, var_decl->token);
            return stmt;
        }
        default: {
            // Expression statement
            AstExpr *expr = parse_expression(p);
            if (!expr) return nullptr;
            
            AstStmt *stmt = ast_stmt_create(AST_STMT_EXPR, expr->token);
            stmt->expr.expression = expr;
            return stmt;
        }
    }
}

AstStmt *
parse_block(Parser *p)
{
    Token brace_token = current(p);
    if (!match(p, Tkn_OpenCurly)) {
        log_error("Expected '{'");
        return nullptr;
    }
    
    AstStmt *block = ast_stmt_create(AST_STMT_BLOCK, brace_token);
    block->block.statements = array_make(AstStmtPtr, 8);
    
    while (!check(p, Tkn_CloseCurly) && !check(p, Tkn_EOT)) {
        skip_terminators(p);
        
        AstStmt *stmt = parse_statement(p);
        if (!stmt) {
            ast_stmt_free(block);
            return nullptr;
        }
        
        array_push(block->block.statements, stmt);
        skip_terminators(p);
    }
    
    if (!match(p, Tkn_CloseCurly)) {
        log_error("Expected '}'");
        ast_stmt_free(block);
        return nullptr;
    }
    
    return block;
}

AstStmt *
parse_if_statement(Parser *p)
{
    Token if_token = current(p);
    advance(p); // consume 'if'
    
    AstStmt *if_stmt = ast_stmt_create(AST_STMT_IF, if_token);
    
    if (!match(p, Tkn_OpenParen)) {
        log_error("Expected '(' after 'if'");
        ast_stmt_free(if_stmt);
        return nullptr;
    }
    
    if_stmt->if_stmt.condition = parse_expression(p);
    if (!if_stmt->if_stmt.condition) {
        ast_stmt_free(if_stmt);
        return nullptr;
    }
    
    if (!match(p, Tkn_CloseParen)) {
        log_error("Expected ')' after if condition");
        ast_stmt_free(if_stmt);
        return nullptr;
    }
    
    if_stmt->if_stmt.then_stmt = parse_statement(p);
    if (!if_stmt->if_stmt.then_stmt) {
        ast_stmt_free(if_stmt);
        return nullptr;
    }
    
    if (match(p, Tkn_ElseKeyword)) {
        if_stmt->if_stmt.else_stmt = parse_statement(p);
        if (!if_stmt->if_stmt.else_stmt) {
            ast_stmt_free(if_stmt);
            return nullptr;
        }
    }
    
    return if_stmt;
}

AstStmt *
parse_while_statement(Parser *p)
{
    Token while_token = current(p);
    advance(p); // consume 'while'
    
    AstStmt *while_stmt = ast_stmt_create(AST_STMT_WHILE, while_token);
    
    if (!match(p, Tkn_OpenParen)) {
        log_error("Expected '(' after 'while'");
        ast_stmt_free(while_stmt);
        return nullptr;
    }
    
    while_stmt->while_stmt.condition = parse_expression(p);
    if (!while_stmt->while_stmt.condition) {
        ast_stmt_free(while_stmt);
        return nullptr;
    }
    
    if (!match(p, Tkn_CloseParen)) {
        log_error("Expected ')' after while condition");
        ast_stmt_free(while_stmt);
        return nullptr;
    }
    
    while_stmt->while_stmt.body = parse_statement(p);
    if (!while_stmt->while_stmt.body) {
        ast_stmt_free(while_stmt);
        return nullptr;
    }
    
    return while_stmt;
}

AstStmt *
parse_for_statement(Parser *p)
{
    Token for_token = current(p);
    advance(p); // consume 'for'
    
    AstStmt *for_stmt = ast_stmt_create(AST_STMT_FOR, for_token);
    
    if (!match(p, Tkn_OpenParen)) {
        log_error("Expected '(' after 'for'");
        ast_stmt_free(for_stmt);
        return nullptr;
    }
    
    // Parse initializer
    if (!check(p, Tkn_Terminator)) {
        for_stmt->for_stmt.init = parse_statement(p);
        if (!for_stmt->for_stmt.init) {
            ast_stmt_free(for_stmt);
            return nullptr;
        }
    }
    match(p, Tkn_Terminator);
    
    // Parse condition
    if (!check(p, Tkn_Terminator)) {
        for_stmt->for_stmt.condition = parse_expression(p);
        if (!for_stmt->for_stmt.condition) {
            ast_stmt_free(for_stmt);
            return nullptr;
        }
    }
    match(p, Tkn_Terminator);
    
    // Parse update
    if (!check(p, Tkn_CloseParen)) {
        for_stmt->for_stmt.update = parse_statement(p);
        if (!for_stmt->for_stmt.update) {
            ast_stmt_free(for_stmt);
            return nullptr;
        }
    }
    
    if (!match(p, Tkn_CloseParen)) {
        log_error("Expected ')' after for clauses");
        ast_stmt_free(for_stmt);
        return nullptr;
    }
    
    for_stmt->for_stmt.body = parse_statement(p);
    if (!for_stmt->for_stmt.body) {
        ast_stmt_free(for_stmt);
        return nullptr;
    }
    
    return for_stmt;
}

AstStmt *
parse_return_statement(Parser *p)
{
    Token ret_token = current(p);
    advance(p); // consume 'ret'
    
    AstStmt *ret_stmt = ast_stmt_create(AST_STMT_RETURN, ret_token);
    
    if (!check(p, Tkn_Terminator) && !check(p, Tkn_CloseCurly)) {
        ret_stmt->return_stmt.value = parse_expression(p);
        if (!ret_stmt->return_stmt.value) {
            ast_stmt_free(ret_stmt);
            return nullptr;
        }
    }
    
    return ret_stmt;
}

// Expression parsing functions with operator precedence
AstExpr *
parse_expression(Parser *p)
{
    return parse_assignment(p);
}

AstExpr *
parse_assignment(Parser *p)
{
    AstExpr *expr = parse_logical_or(p);
    if (!expr) return nullptr;
    
    if (match(p, Tkn_Equal) || match(p, Tkn_AddEqual) || 
        match(p, Tkn_SubEqual) || match(p, Tkn_MultEqual) || 
        match(p, Tkn_DivEqual)) {
        Token operator = previous(p);
        AstExpr *value = parse_assignment(p);
        if (!value) {
            ast_expr_free(expr);
            return nullptr;
        }
        
        AstExpr *assign = ast_expr_create(AST_EXPR_ASSIGN, operator);
        assign->assign.target = expr;
        assign->assign.operator = operator;
        assign->assign.value = value;
        return assign;
    }
    
    return expr;
}

AstExpr *
parse_logical_or(Parser *p)
{
    AstExpr *expr = parse_logical_and(p);
    if (!expr) return nullptr;
    
    while (match(p, Tkn_OrKeyword)) {
        Token operator = previous(p);
        AstExpr *right = parse_logical_and(p);
        if (!right) {
            ast_expr_free(expr);
            return nullptr;
        }
        
        AstExpr *binary = ast_expr_create(AST_EXPR_BINARY, operator);
        binary->binary.left = expr;
        binary->binary.operator = operator;
        binary->binary.right = right;
        expr = binary;
    }
    
    return expr;
}

AstExpr *
parse_logical_and(Parser *p)
{
    AstExpr *expr = parse_equality(p);
    if (!expr) return nullptr;
    
    while (match(p, Tkn_AndKeyword)) {
        Token operator = previous(p);
        AstExpr *right = parse_equality(p);
        if (!right) {
            ast_expr_free(expr);
            return nullptr;
        }
        
        AstExpr *binary = ast_expr_create(AST_EXPR_BINARY, operator);
        binary->binary.left = expr;
        binary->binary.operator = operator;
        binary->binary.right = right;
        expr = binary;
    }
    
    return expr;
}

AstExpr *
parse_equality(Parser *p)
{
    AstExpr *expr = parse_comparison(p);
    if (!expr) return nullptr;
    
    while (match(p, Tkn_NotEqual) || match(p, Tkn_EqualEqual)) {
        Token operator = previous(p);
        AstExpr *right = parse_comparison(p);
        if (!right) {
            ast_expr_free(expr);
            return nullptr;
        }
        
        AstExpr *binary = ast_expr_create(AST_EXPR_BINARY, operator);
        binary->binary.left = expr;
        binary->binary.operator = operator;
        binary->binary.right = right;
        expr = binary;
    }
    
    return expr;
}

AstExpr *
parse_comparison(Parser *p)
{
    AstExpr *expr = parse_term(p);
    if (!expr) return nullptr;
    
    while (match(p, Tkn_Greater) || match(p, Tkn_GreaterEql) || 
           match(p, Tkn_Less) || match(p, Tkn_LessEql)) {
        Token operator = previous(p);
        AstExpr *right = parse_term(p);
        if (!right) {
            ast_expr_free(expr);
            return nullptr;
        }
        
        AstExpr *binary = ast_expr_create(AST_EXPR_BINARY, operator);
        binary->binary.left = expr;
        binary->binary.operator = operator;
        binary->binary.right = right;
        expr = binary;
    }
    
    return expr;
}

AstExpr *
parse_term(Parser *p)
{
    AstExpr *expr = parse_factor(p);
    if (!expr) return nullptr;
    
    while (match(p, Tkn_MinusOperator) || match(p, Tkn_PlusOperator)) {
        Token operator = previous(p);
        AstExpr *right = parse_factor(p);
        if (!right) {
            ast_expr_free(expr);
            return nullptr;
        }
        
        AstExpr *binary = ast_expr_create(AST_EXPR_BINARY, operator);
        binary->binary.left = expr;
        binary->binary.operator = operator;
        binary->binary.right = right;
        expr = binary;
    }
    
    return expr;
}

AstExpr *
parse_factor(Parser *p)
{
    AstExpr *expr = parse_unary(p);
    if (!expr) return nullptr;
    
    while (match(p, Tkn_DivOperator) || match(p, Tkn_MultOperator) || match(p, Tkn_Mod)) {
        Token operator = previous(p);
        AstExpr *right = parse_unary(p);
        if (!right) {
            ast_expr_free(expr);
            return nullptr;
        }
        
        AstExpr *binary = ast_expr_create(AST_EXPR_BINARY, operator);
        binary->binary.left = expr;
        binary->binary.operator = operator;
        binary->binary.right = right;
        expr = binary;
    }
    
    return expr;
}

AstExpr *
parse_unary(Parser *p)
{
    if (match(p, Tkn_Not) || match(p, Tkn_MinusOperator)) {
        Token operator = previous(p);
        AstExpr *operand = parse_unary(p);
        if (!operand) return nullptr;
        
        AstExpr *unary = ast_expr_create(AST_EXPR_UNARY, operator);
        unary->unary.operator = operator;
        unary->unary.operand = operand;
        return unary;
    }
    
    return parse_call(p);
}

AstExpr *
parse_call(Parser *p)
{
    AstExpr *expr = parse_primary(p);
    if (!expr) return nullptr;
    
    while (true) {
        if (match(p, Tkn_OpenParen)) {
            // Function call
            AstExpr *call = ast_expr_create(AST_EXPR_CALL, expr->token);
            call->call.callee = expr;
            call->call.arguments = array_make(AstExprPtr, 4);
            
            if (!check(p, Tkn_CloseParen)) {
                do {
                    AstExpr *arg = parse_expression(p);
                    if (!arg) {
                        ast_expr_free(call);
                        return nullptr;
                    }
                    array_push(call->call.arguments, arg);
                } while (match(p, Tkn_Comma));
            }
            
            if (!match(p, Tkn_CloseParen)) {
                log_error("Expected ')' after arguments");
                ast_expr_free(call);
                return nullptr;
            }
            
            expr = call;
        } else if (match(p, Tkn_Dot)) {
            // Member access
            if (!check(p, Tkn_Identifier)) {
                log_error("Expected property name after '.'");
                ast_expr_free(expr);
                return nullptr;
            }
            
            Token member = current(p);
            advance(p);
            
            AstExpr *member_expr = ast_expr_create(AST_EXPR_MEMBER, member);
            member_expr->member.object = expr;
            member_expr->member.member = member;
            expr = member_expr;
        } else {
            break;
        }
    }
    
    return expr;
}

AstExpr *
parse_primary(Parser *p)
{
    if (match(p, Tkn_TrueLiteral) || match(p, Tkn_FalseLiteral) || 
        match(p, Tkn_NilLiteral) || match(p, Tkn_IntegerLiteral) || 
        match(p, Tkn_FloatLiteral) || match(p, Tkn_StringLiteral) || 
        match(p, Tkn_CharLiteral)) {
        Token literal = previous(p);
        AstExpr *expr = ast_expr_create(AST_EXPR_LITERAL, literal);
        expr->literal.value = literal;
        return expr;
    }
    
    if (match(p, Tkn_Identifier)) {
        Token identifier = previous(p);
        AstExpr *expr = ast_expr_create(AST_EXPR_IDENTIFIER, identifier);
        expr->identifier.name = identifier;
        return expr;
    }
    
    if (match(p, Tkn_OpenParen)) {
        AstExpr *expr = parse_expression(p);
        if (!expr) return nullptr;
        
        if (!match(p, Tkn_CloseParen)) {
            log_error("Expected ')' after expression");
            ast_expr_free(expr);
            return nullptr;
        }
        
        return expr;
    }
    
    log_error("Expected expression");
    return nullptr;
}

// Type parsing function
AstType *
parse_type(Parser *p)
{
    Token type_token = current(p);
    
    if (match(p, Tkn_IntKeyword) || match(p, Tkn_UIntKeyword) || 
        match(p, Tkn_FltKeyword) || match(p, Tkn_BoolKeyword) || 
        match(p, Tkn_CharKeyword)) {
        AstType *type = ast_type_create(AST_TYPE_BASIC, type_token);
        
        switch (type_token.type) {
            case Tkn_IntKeyword:
                type->basic.base_type = BT_Int;
                break;
            case Tkn_UIntKeyword:
                type->basic.base_type = BT_UInt;
                break;
            case Tkn_FltKeyword:
                type->basic.base_type = BT_Float;
                break;
            case Tkn_BoolKeyword:
                type->basic.base_type = BT_Bool;
                break;
            case Tkn_CharKeyword:
                type->basic.base_type = BT_Char;
                break;
            default:
                type->basic.base_type = BT_Invalid;
                break;
        }
        return type;
    }
    
    if (match(p, Tkn_Identifier)) {
        AstType *type = ast_type_create(AST_TYPE_BASIC, type_token);
        type->user_defined.name = type_token;
        return type;
    }
    
    if (match(p, Tkn_OpenSQRBrackets)) {
        // Array type: [size]element_type
        AstType *array_type = ast_type_create(AST_TYPE_ARRAY, type_token);
        
        if (!check(p, Tkn_CloseSQRBrackets)) {
            array_type->array.size = parse_expression(p);
            if (!array_type->array.size) {
                ast_type_free(array_type);
                return nullptr;
            }
        }
        
        if (!match(p, Tkn_CloseSQRBrackets)) {
            log_error("Expected ']' after array size");
            ast_type_free(array_type);
            return nullptr;
        }
        
        array_type->array.element_type = parse_type(p);
        if (!array_type->array.element_type) {
            ast_type_free(array_type);
            return nullptr;
        }
        
        return array_type;
    }
    
    log_error("Expected type");
    return nullptr;
}
