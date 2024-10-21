#include "token.h"

// TODO: convert tokens to cstring funcs

cstr
tkn_type_describe(const TknType type)
{
    switch (type)
    {
        case Tkn_LetKeyword: return "let";
        case Tkn_DeferKeyword: return "defer";
        case Tkn_VariantKeyword: return "variant";
        case Tkn_FallKeyword: return "fall";
        case Tkn_RightShift: return ">>";
        case Tkn_LeftShift: return "<<";
        case Tkn_BitwiseXor: return "^";
        case Tkn_BitwiseOr: return "|";
        case Tkn_BitwiseAnd: return "&";
        case Tkn_NewKeyword: return "new";
        case Tkn_AsKeyword: return "as";
        case Tkn_DeleteKeyword: return "delete";
        case Tkn_CharKeyword: return "char";
        case Tkn_Identifier: return "identifier";
        case Tkn_BuiltinId: return "builtin_id";
        case Tkn_Equal: return "assign'='";
        case Tkn_IntegerLiteral: return "integer";
        case Tkn_Colon: return "colon':'";
        case Tkn_StringLiteral: return "string";
        case Tkn_FnKeyword: return "function'fn'";
        case Tkn_PlusOperator: return "plus'+'";
        case Tkn_MinusOperator: return "minus'-'";
        case Tkn_MultOperator: return "star'*'";
        case Tkn_DivOperator: return "divide'/'";
        case Tkn_OpenParen: return "openparen'('";
        case Tkn_CloseParen: return "closeparen')'";
        case Tkn_OpenCurly: return "opencurlybrkt'{'";
        case Tkn_CloseCurly: return "closecurlybrkt'}'";
        case Tkn_OpenSQRBrackets: return "opensqrbrkt']'";
        case Tkn_CloseSQRBrackets: return "closesqrbrkt'['";
        case Tkn_CharLiteral: return "char";
        case Tkn_RetKeyword: return "return";
        case Tkn_ImportKeyword: return "import";
        case Tkn_IfKeyword: return "if";
        case Tkn_ElseKeyword: return "else";
        case Tkn_ForKeyword: return "for";
        case Tkn_WhileKeyword: return "while";
        case Tkn_Greater: return "greater'>'";
        case Tkn_Less: return "less'<'";
        case Tkn_TrueLiteral: return "bool:true";
        case Tkn_FalseLiteral: return "bool:false";
        case Tkn_Dot: return "dot'.'";
        case Tkn_Mod: return "%";
        case Tkn_Not: return "not'!'";
        case Tkn_AndKeyword: return "and";
        case Tkn_OrKeyword: return "or";
        case Tkn_FloatLiteral: return "float";
        case Tkn_Comma: return "comma','";
        case Tkn_PubKeyword: return "public";
        case Tkn_NotEqual: return "`!=` not eql"; return "char_word";
        case Tkn_FltKeyword: return "float_word";
        case Tkn_IntKeyword: return "int_word";
        case Tkn_SwitchKeyword: return "switch";
        case Tkn_BoolKeyword: return "bool_word";
        case Tkn_EqualEqual: return "equality";
        case Tkn_BreakKeyword: return "break";
        case Tkn_AddEqual: return "add_equal";
        case Tkn_DivEqual: return "divide_equal";
        case Tkn_MultEqual: return "multiply_equal";
        case Tkn_SubEqual: return "subtract_equal";
        case Tkn_StructKeyword: return "struct";
        case Tkn_RefKeyword: return "ref";
        case Tkn_EnumKeyword: return "enum";
        case Tkn_NilLiteral: return "nil (null)";
        case Tkn_UIntKeyword: return "uint";
        case Tkn_GreaterEql: return ">=";
        case Tkn_LessEql: return "<=";
        case Tkn_Terminator: return "terminator";
        case Tkn_EOT: return "End OF Tokens";
        case Tkn_COUNT: break;
    }

    return "UNKNOWN";
}

cstr
lexer_err_msg(const LexErr error)
{
    switch (error)
    {
        case LE_OUT_OF_MEMORY: return "Out of memory";
        case LE_LEXER_INVALID_CHAR: return "Invalid character";
        case LE_TOO_LONG_IDENTIFIER: return "Identifier is too long";
        case LE_TOO_LONG_NUMBER: return "Number is too long";
        case LE_TOO_LONG_STRING: return "String is too long";
        case LE_NOT_CLOSED_CHAR: return "The char is not closed.";
        case LE_NOT_CLOSED_STRING: return "The cstr is not closed.";
        case LE_END_OF_FILE: return "reached end of file.";
        case LE_FILE_EMPTY: return "The file is empty.";
        case LE_BAD_TOKEN_AT_GLOBAL: return "Found global token at its forbidden scope";
        case LE_TABS: return "Tabs '\\t' are unsupported";
        case LE_NOT_VALID_ESCAPE_CHAR: return "Invalid escaped char";
        case LE_NOT_CLOSED_COMMENT: return "Comment not closed";
        case LE_UNSUPPORTED: break;
        case LE_UNKNOWN: break;
    }
    TODO("error msg implementation.");
    return "TODO: error msg implementation.";
}

cstr
lexer_err_advice(const LexErr error)
{
    switch (error)
    {
        case LE_NOT_VALID_ESCAPE_CHAR: return "Change the letter after \\";
        case LE_NOT_CLOSED_COMMENT: return "Close the comment with delimiter";
        case LE_LEXER_INVALID_CHAR: return "remove this character";
        case LE_OUT_OF_MEMORY: return "The compiler needs more memory";
        case LE_TOO_LONG_IDENTIFIER: return "Identifier must not exceed 100 characters";
        case LE_TOO_LONG_NUMBER: return "Number must not exceed 100 digits";
        case LE_TOO_LONG_STRING: return "String must not exceed (uint_MAX / 2) characters";
        case LE_NOT_CLOSED_CHAR: return "Close the char with a quote";
        case LE_NOT_CLOSED_STRING: return "Close the cstr with a double quote";
        case LE_END_OF_FILE: return "Needs more code for compiling";
        case LE_FILE_EMPTY: return "Do not compile empty files";
        case LE_BAD_TOKEN_AT_GLOBAL: return "Do not put this token in global scope";
        case LE_TABS: return "Convert the tabs to spaces";
        case LE_UNSUPPORTED: break;
        case LE_UNKNOWN: break;
    }
    TODO("error msg implementation.");
    return "TODO: error msg implementation.";
}
