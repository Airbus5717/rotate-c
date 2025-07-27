#pragma once

#include "token.h"
#include "../include/arraylist.h"

// NOTE(5717): not to be confused with TknIdx
// Type Index will pointer multiple symbol tables and will differ by the type
typedef uint TypeIndex;  // Type index
typedef uint ExprIdx;    // Expression index
typedef uint ExprLocIdx; // Expression location index

typedef enum
{
    // TODO: if needs any more base types, add them here
    BT_Invalid,
    BT_Void, // NOTE(5717): ONLY ALLOWED FOR FUNCTIONS
    BT_Int,
    BT_UInt,
    BT_Float,
    BT_Char, // u8
    BT_Bool,
    BT_Array, // strings are char arrays
    BT_Struct,
    BT_Enum, // a member
    BT_Id,   // Identifier, TO BE VALIDATED
    BT_TBD,  // TO BE DETERMINED
} BaseType;

// Forward declarations for AST nodes
typedef struct AstNode AstNode;
typedef struct AstType AstType;
typedef struct AstExpr AstExpr;
typedef struct AstStmt AstStmt;
typedef struct AstDecl AstDecl;

// Pointer typedefs for arrays
typedef AstNode* AstNodePtr;
typedef AstType* AstTypePtr;
typedef AstExpr* AstExprPtr;
typedef AstStmt* AstStmtPtr;
typedef AstDecl* AstDeclPtr;

// AST Node Types
typedef enum
{
    AST_DECL_IMPORT,
    AST_DECL_FUNCTION,
    AST_DECL_VARIABLE,
    AST_DECL_STRUCT,
    AST_DECL_ENUM,
    
    AST_STMT_EXPR,
    AST_STMT_DECL,
    AST_STMT_IF,
    AST_STMT_WHILE,
    AST_STMT_FOR,
    AST_STMT_RETURN,
    AST_STMT_BREAK,
    AST_STMT_DEFER,
    AST_STMT_BLOCK,
    
    AST_EXPR_LITERAL,
    AST_EXPR_IDENTIFIER,
    AST_EXPR_BINARY,
    AST_EXPR_UNARY,
    AST_EXPR_CALL,
    AST_EXPR_MEMBER,
    AST_EXPR_ASSIGN,
    
    AST_TYPE_BASIC,
    AST_TYPE_ARRAY,
    AST_TYPE_FUNCTION,
    AST_TYPE_STRUCT,
    AST_TYPE_ENUM,
} AstNodeType;

// Generate array types for AST node pointers
generate_array_type(AstNodePtr);
generate_array_type(AstExprPtr);
generate_array_type(AstStmtPtr);
generate_array_type(AstDeclPtr);
generate_array_type(AstTypePtr);

// AST Type node
typedef struct AstType
{
    AstNodeType kind;
    Token token;
    
    union {
        struct {
            BaseType base_type;
        } basic;
        
        struct {
            AstType *element_type;
            AstExpr *size;
        } array;
        
        struct {
            Array(AstTypePtr) param_types;
            AstType *return_type;
        } function;
        
        struct {
            Token name;
        } user_defined;
    };
} AstType;

// AST Expression node
typedef struct AstExpr
{
    AstNodeType kind;
    Token token;
    AstType *type;
    
    union {
        struct {
            Token value;
        } literal;
        
        struct {
            Token name;
        } identifier;
        
        struct {
            AstExpr *left;
            Token operator;
            AstExpr *right;
        } binary;
        
        struct {
            Token operator;
            AstExpr *operand;
        } unary;
        
        struct {
            AstExpr *callee;
            Array(AstExprPtr) arguments;
        } call;
        
        struct {
            AstExpr *object;
            Token member;
        } member;
        
        struct {
            AstExpr *target;
            Token operator;
            AstExpr *value;
        } assign;
    };
} AstExpr;

// AST Statement node
typedef struct AstStmt
{
    AstNodeType kind;
    Token token;
    
    union {
        struct {
            AstExpr *expression;
        } expr;
        
        struct {
            AstDecl *declaration;
        } decl;
        
        struct {
            AstExpr *condition;
            AstStmt *then_stmt;
            AstStmt *else_stmt;
        } if_stmt;
        
        struct {
            AstExpr *condition;
            AstStmt *body;
        } while_stmt;
        
        struct {
            AstStmt *init;
            AstExpr *condition;
            AstStmt *update;
            AstStmt *body;
        } for_stmt;
        
        struct {
            AstExpr *value;
        } return_stmt;
        
        struct {
            AstStmt *statement;
        } defer_stmt;
        
        struct {
            Array(AstStmtPtr) statements;
        } block;
    };
} AstStmt;

// AST Declaration node
typedef struct AstDecl
{
    AstNodeType kind;
    Token token;
    
    union {
        struct {
            Token alias;
            Token module_path;
        } import;
        
        struct {
            Token name;
            Array(AstDeclPtr) parameters;
            AstType *return_type;
            AstStmt *body;
        } function;
        
        struct {
            Token name;
            AstType *type;
            AstExpr *initializer;
            bool is_constant;
        } variable;
        
        struct {
            Token name;
            Array(AstDeclPtr) fields;
        } struct_decl;
        
        struct {
            Token name;
            Array(AstDeclPtr) members;
        } enum_decl;
    };
} AstDecl;

// Main AST node
typedef struct AstNode
{
    AstNodeType kind;
    Token token;
    
    union {
        AstDecl *decl;
        AstStmt *stmt;
        AstExpr *expr;
        AstType *type;
    };
} AstNode;

// AST Program - root node
typedef struct AstProgram
{
    Array(AstDeclPtr) declarations;
} AstProgram;
