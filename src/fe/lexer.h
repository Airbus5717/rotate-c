#pragma once

#include "token.h"

generate_array_type(Token);

typedef struct Lexer
{
    // lexer state variables
    uint index, len, line, begin_tkn_line, file_length;
    File *file; // not owned by the lexer
    LexErr error;
    uint save_index, save_line;
    TknType prev;
    ArrayList(Token) tokens;
} Lexer;

// Lexer API
Lexer lexer_init(File *);
void lexer_deinit(Lexer *);
ArrayList(Token) lexer_get_tokens(Lexer *lexer);
u8 lexer_lex(Lexer *);
void lexer_save_log(Lexer *, FILE *);
// internal methods are in lexer.c
