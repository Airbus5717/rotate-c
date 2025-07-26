#include "../include/common.h"
#include "../fe/token.h"
#include <time.h>
#include <math.h>

#define TIME_BUFFER_SIZE 20

internal void 
log_message(const char *level, const char *color, cstr message)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_buffer[TIME_BUFFER_SIZE];
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
    if (!src || length == 0) {
        return NULL;
    }
    
    char *res = mem_alloc(length + 1);
    memcpy(res, src, length);
    res[length] = '\0';
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
    if (num == 0) {
        return 1;
    }
    return (uint)floor(log10l(num) + 1);
}

u8
bit_set(const u8 field, const u8 n)
{
    if (n >= 8) {
        return field;
    }
    return (field | (u8)((u8)1 << n));
}

u8
bit_clear(const u8 field, const u8 n)
{
    if (n >= 8) {
        return field;
    }
    return (field & (u8)(~((u8)1 << n)));
}

bool
bit_is_set(const u8 field, const u8 n)
{
    if (n >= 8) {
        return false;
    }
    return (((field >> n) & 1) != 0);
}
