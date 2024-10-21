#pragma once

#include "../include/arraylist.h"
#include "../include/common.h"
#include "../include/file.h"

typedef uint TknIdx;

typedef enum
{
    Tkn_Identifier,       // ids
    Tkn_BuiltinId,        // @id
    Tkn_AsKeyword,        // 'as'
    Tkn_LetKeyword,       // 'let'
    Tkn_DeleteKeyword,    // 'del'
    Tkn_NewKeyword,       // 'new'
    Tkn_Equal,            // =
    Tkn_IntegerLiteral,   // refers to 10 digits ints
    Tkn_IntKeyword,       // 'int'
    Tkn_UIntKeyword,      // 'uint'
    Tkn_FloatLiteral,     // refer to floats
    Tkn_FltKeyword,       // 'flt'
    Tkn_StringLiteral,    // refer to strings
    Tkn_CharLiteral,      // refers to chars
    Tkn_CharKeyword,      // 'char'
    Tkn_TrueLiteral,      // 'true'
    Tkn_FalseLiteral,     // 'false'
    Tkn_BoolKeyword,      // 'bool'
    Tkn_Terminator,       // ; | '\n'
    Tkn_Colon,            // :
    Tkn_FnKeyword,        // 'fn'
    Tkn_PlusOperator,     // +
    Tkn_MinusOperator,    // -
    Tkn_MultOperator,     // *
    Tkn_DivOperator,      // /
    Tkn_OpenParen,        // (
    Tkn_CloseParen,       // )
    Tkn_OpenCurly,        // {
    Tkn_CloseCurly,       // }
    Tkn_OpenSQRBrackets,  // [
    Tkn_CloseSQRBrackets, // ]
    Tkn_RetKeyword,       // 'ret'
    Tkn_ImportKeyword,    // 'import'
    Tkn_IfKeyword,        // 'if'
    Tkn_ElseKeyword,      // 'else'
    Tkn_ForKeyword,       // 'for'
    Tkn_WhileKeyword,     // 'while'
    Tkn_Greater,          // >
    Tkn_GreaterEql,       // >=
    Tkn_Less,             // <
    Tkn_LessEql,          // <=
    Tkn_Dot,              // .
    Tkn_Mod,              // %
    Tkn_Not,              // "!"
    Tkn_NotEqual,         // "!="
    Tkn_AndKeyword,       // 'and' logical and
    Tkn_BitwiseAnd,       // &
    Tkn_BitwiseOr,        // |
    Tkn_BitwiseXor,       // ^
    Tkn_LeftShift,        // <<
    Tkn_RightShift,       // >>
    Tkn_OrKeyword,        // 'or' logical or
    Tkn_Comma,            // ,
    Tkn_PubKeyword,       // 'pub'
    Tkn_SwitchKeyword,    // 'switch'
    Tkn_EnumKeyword,      // 'enum'
    Tkn_EqualEqual,       // ==
    Tkn_BreakKeyword,     // 'break' // only loops
    Tkn_FallKeyword,      // 'fall' // only switch statements
    Tkn_AddEqual,         // +=
    Tkn_SubEqual,         // -=
    Tkn_MultEqual,        // *=
    Tkn_DivEqual,         // /=
    Tkn_StructKeyword,    // 'struct'
    Tkn_DeferKeyword,     // 'defer' cleanup at end of scope
    Tkn_VariantKeyword,   // 'variant' basically tagged union
    Tkn_RefKeyword,       // 'ref' // TODO later
    Tkn_NilLiteral,       // `nil` basically null
    Tkn_EOT,              // END OF TOKENS (last token type in list)

    // WARN: Never use this as a token type
    Tkn_COUNT, // Used for counting the number of members of the enum

} TknType; // enum TknType

cstr tkn_type_describe(const TknType type);

typedef struct
{
    uint index, length, line;
    TknType type;
} Token;

typedef enum
{
    // Unknown token/error (default)
    LE_UNKNOWN,
    LE_OUT_OF_MEMORY,
    LE_LEXER_INVALID_CHAR,
    LE_TOO_LONG_IDENTIFIER, // an ID can have up to a specific length
    LE_TOO_LONG_NUMBER,
    LE_TOO_LONG_STRING,
    LE_TABS,                  // Tabs are not supported
    LE_NOT_CLOSED_CHAR,       // Single quote not closed
    LE_NOT_VALID_ESCAPE_CHAR, // Double quote not closed
    LE_NOT_CLOSED_STRING,
    LE_FILE_EMPTY,          // File empty error
    LE_END_OF_FILE,         // End of file error
    LE_BAD_TOKEN_AT_GLOBAL, // forbidden token in global scope
    LE_NOT_CLOSED_COMMENT,
    LE_UNSUPPORTED,
} LexErr; // enum LexErr

cstr get_keyword_or_type(const Token);
cstr lexer_err_advice(const LexErr);
cstr lexer_err_msg(const LexErr);
bool is_token_type_length_variable(TknType);
bool is_token_a_number(TknType);
void log_token(FILE *const, const Token, cstr);
