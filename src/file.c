#include "include/file.h"
#include "include/defines.h"

/// NOTE:
/// the whole file will be read at once
/// to avoid potential problems with the
/// filesystem during reading as developers
/// may modify the files during reading
File
file_read(cstr name)
{
    const usize len = strlen(name);

    cstr file_ext = &(name)[len - 3];
    if (strcmp(file_ext, ".vr") != 0)
    {
        fprintf(stderr, "%s%serror:%s file name: `%s` must end with `.vr`%s\n", BOLD, LRED, WHITE,
                name, RESET);

        return (File){nullptr, nullptr, 0, failure};
    }

    

    // open file
    FILE *file = fopen(name, "rb");
    if (!file)
    {
        // display error message if file does not exist
        log_error("File does not exist");
        return (File){nullptr, nullptr, 0, failure};
    }

    // Calculate the file length
    fseek(file, 0, SEEK_END);
    if (!ftell(file))
    {
        log_error("File is empty");
        fclose(file);
        return (File){nullptr, nullptr, 0, failure};
    }

    // NOTE: use max unsigned int possible to avoid overflow
    const usize length = (usize)ftell(file);

    // NOTE: hardcoded null-terminators
    if (length > (RUINT_MAX - EXTRA_NULL_TERMINATORS))
    {
        log_error("File is too large");
        fclose(file);
        return (File){nullptr, nullptr, 0, failure};
    }
    // rewind the fseek to the beginning of the file
    rewind(file);

    // Read the file into a buffer
    char *buffer = malloc(length + EXTRA_NULL_TERMINATORS);
    if (!buffer) exit_error("Memory allocation failure");

    // get file contents
    if (fread(buffer, sizeof(char), length, file) != length)
    {
        log_error("Read file error");
        fclose(file);
        free(buffer);
        return (File){nullptr, nullptr, 0, failure};
    }

    // add nullptr charactor (EXTRA_NULL_TERMINATORS for extra safety)
    for (u8 i = 0; i < EXTRA_NULL_TERMINATORS; i++)
        buffer[length + i] = '\0';

    // simple validator (check first char if it is a visible ascii or is_space(without tabs))
    char c = buffer[0];
    if (!isspace(c) && !isprint(c))
    {
        log_error("Only ascii text files are supported for compilation");
        fclose(file);
        free(buffer);
        return (File){nullptr, nullptr, 0, failure};
    }

    // Close the file
    fclose(file);

    uint _len  = (uint)length;
    File res = (File){name, buffer, _len, success};
    return res;
}

void
file_free(File *file)
{
    mem_free(file->contents);
}
