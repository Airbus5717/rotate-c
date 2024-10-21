#pragma once

#include "defines.h"
#include "mem.h"

typedef enum
{
    ST_UNKNOWN,
    ST_FILE,
    ST_LEXER,
    ST_PARSER,
    ST_TCHECKER,
    ST_LOGGER,
    // TODO: add the rest
} Stage;
cstr main_err(Stage);
/*
 *  Utilites
 */

// Loggin
void log_stage(cstr);
void log_error(cstr);
void exit_error(cstr);
void log_debug(cstr);
void log_info(cstr);
void log_warn(cstr);
void log_error_unknown_flag(cstr str);

// cstr utils
bool string_cmp(cstr, cstr);

char *string_dup(cstr, const usize);
//
uint get_digits_from_number(const uint);
// bitwise operations
u8 bit_set(const u8 field, const u8 n);
u8 bit_clear(const u8 field, const u8 n);
u8 bit_is_set(const u8 field, const u8 n);
