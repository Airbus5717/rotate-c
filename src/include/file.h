#pragma once

#include "common.h"

typedef enum
{
    failure = 0,
    success,
} valid;

typedef struct
{
    cstr name;
    char *contents;
    const uint length; // contents length
    valid valid_code;
} File;

File file_read(cstr name);
void file_free(File *);
