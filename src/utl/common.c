#include "../include/common.h"
#include "../fe/token.h"
#include <time.h> // For timestamps in logging

// Centralized logging function
static void log_message(const char *level, const char *color, cstr message)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_buffer[20];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", t);

    fprintf(stderr, "[%s%s%s] [%s]: %s\n", color, level, RESET, time_buffer, message);
}

void log_stage(cstr str)
{
    log_message("STAGE", LRED BOLD, str);
}

void log_error(cstr str)
{
    log_message("ERROR", LRED BOLD, str);
}

void exit_error(cstr str)
{
    log_error(str);
    exit(1);
}

void log_warn(cstr str)
{
    log_message("WARN", LYELLOW BOLD, str);
}

void log_debug(cstr str)
{
#if DEBUG
    log_message("DEBUG", LYELLOW BOLD, str);
#else
    UNUSED(str);
#endif
}

void log_info(cstr str)
{
    log_message("INFO", LGREEN BOLD, str);
}

// Memory allocation

void *mem_alloc(usize size)
{
    void *result = malloc(size);
    if (!result)
    {
        log_error("Memory allocation failed");
        exit(1);
    }
    return result;
}

void *mem_resize(void *blk, usize size)
{
    void *result = realloc(blk, size);
    if (!result)
    {
        log_error("Memory reallocation failed");
        exit(1);
    }
    return result;
}

void mem_free(void *blk)
{
    if (blk)
    {
        free(blk);
    }
}

// NOTE: func definition in ./frontend/include/lexer.hpp
void
log_token(FILE *output, const Token tkn, cstr str)
{
    fprintf(output, "[TOKEN]: idx: %u, len: %u, type: %s, val: `%.*s`\n", tkn.index, tkn.length,
            tkn_type_describe(tkn.type), tkn.length, str + tkn.index);
}

void
log_error_unknown_flag(cstr str)
{
    fprintf(stderr, "[%sWARN%s] : Ignored flag: `%s`\n", LYELLOW, RESET, str);
}

char *
string_dup(cstr src, const usize length)
{
    char *res = malloc(length);
    ASSERT_NULL(res, "failed mem allocation");
    for (usize i = 0; i < length; ++i)
    {
        res[i] = src[i];
    }
    return res;
}

bool
string_cmp(cstr restrict a, cstr restrict b)
{
    bool res = (strcmp(a, b) == 0);
    return res;
}

uint
get_digits_from_number(const uint num)
{
    // TODO: Test this algorithm
    return (uint)floor(log10l(num) + 1);
}

u8
bit_set(const u8 field, const u8 n)
{
    // NOTE(5717): n should be [1:8],
    // otherwise it is ignored
    return (field | (u8)((u8)1 << n));
}

u8
bit_clear(const u8 field, const u8 n)
{
    // NOTE(5717): n should be [1:8]
    // otherwise it is ignored
    return (field & (u8)(~((u8)1 << n)));
}

u8
bit_is_set(const u8 field, const u8 n)
{
    // NOTE(5717): returns non zero for true else 0 for false
    return ((field >> n) & 1);
}
