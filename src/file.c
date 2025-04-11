#include "include/file.h"
#include "include/defines.h"
#include <sys/stat.h>

/// NOTE:
/// the whole file will be read at once
/// to avoid potential problems with the
/// filesystem during reading as developers
/// may modify the files during reading
File
file_read(cstr name)
{
    const usize len = strlen(name);

    // Validate file name length
    if (len < 3)
    {
        log_error("File name is too short to have a valid extension");
        return (File){nullptr, nullptr, 0, failure};
    }

    cstr file_ext = &(name)[len - 3];
    if (strcmp(file_ext, ".vr") != 0)
    {
        log_error("File name must end with .vr");
        return (File){nullptr, nullptr, 0, failure};
    }

    // Open file
    FILE *file = fopen(name, "rb");
    if (!file)
    {
        log_error("File does not exist");
        return (File){nullptr, nullptr, 0, failure};
    }

    // Get file size using fstat
    struct stat file_stat;
    if (fstat(fileno(file), &file_stat) != 0)
    {
        log_error("Failed to get file size");
        fclose(file);
        return (File){nullptr, nullptr, 0, failure};
    }

    const usize length = (usize)file_stat.st_size;

    if (length == 0)
    {
        log_error("File is empty");
        fclose(file);
        return (File){nullptr, nullptr, 0, failure};
    }

    if (length > (RUINT_MAX - EXTRA_NULL_TERMINATORS))
    {
        log_error("File is too large");
        fclose(file);
        return (File){nullptr, nullptr, 0, failure};
    }

    // Allocate buffer
    char *buffer = malloc(length + EXTRA_NULL_TERMINATORS);
    if (!buffer)
    {
        fclose(file);
        exit_error("Memory allocation failure");
    }

    // Read file contents
    if (fread(buffer, sizeof(char), length, file) != length)
    {
        log_error("Read file error");
        fclose(file);
        free(buffer);
        return (File){nullptr, nullptr, 0, failure};
    }

    // Add null terminators
    for (u8 i = 0; i < EXTRA_NULL_TERMINATORS; i++)
        buffer[length + i] = '\0';

    // Validate first character
    char c = buffer[0];
    if (!isspace(c) && !isprint(c))
    {
        log_error("Only ASCII text files are supported for compilation");
        fclose(file);
        free(buffer);
        return (File){nullptr, nullptr, 0, failure};
    }

    // Close file
    fclose(file);

    File res = (File){name, buffer, (uint)length, success};
    return res;
}

void
file_free(File *file)
{
    if (file && file->contents)
    {
        mem_free(file->contents);
        file->contents = nullptr;
    }
}
