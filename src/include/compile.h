#pragma once

#include "common.h"

void print_version_and_exit(void);

typedef struct
{
    i32 argc;
    i8 **argv;
    cstr filename;
    bool debug_info;
    bool debug_symbols;
    bool timer;
    bool lex_only;
    Stage st;
} compile_options;

typedef struct
{
    uint file_size;
    uint token_count;
    u8 status;
} compile_info_stats;

compile_info_stats compile(compile_options *options);
compile_options compile_options_new(const i32 argc, i8 **argv);
