#include "include/compile.h"
#include "include/common.h"
#include "include/file.h"
#include "include/log.h"

#include "fe/parser.h"

#define MIN_TOKEN_COUNT 2u
#define OUTPUT_LOG_FILE "output.org"

internal u8
compile_file_stage(compile_options *options, File *file)
{
    options->st = ST_FILE;
    File temp_file = file_read(options->filename);
    ASSERT_RET_FAIL(temp_file.valid_code == success, "File read error: Unable to read the file or invalid format");
    memcpy(file, &temp_file, sizeof(File));
    return SUCCESS;
}

internal u8
compile_lexer_stage(compile_options *options, File *file, Lexer *lexer, compile_info_stats *stats)
{
    options->st = ST_LEXER;
    *lexer = lexer_init(file);
    u8 status = lexer_lex(lexer);

    if (lexer_get_tokens(lexer)->count < MIN_TOKEN_COUNT) {
        log_error("file is empty");
    }

    if (status == FAILURE) {
        return FAILURE;
    }

    stats->token_count = array_length(lexer->tokens);
    return SUCCESS;
}

internal u8
compile_parser_stage(compile_options *options, Lexer *lexer, Parser *parser)
{
    options->st = ST_PARSER;
    *parser = parser_init(lexer);

    if (!options->lex_only) {
        u8 status = parser_parse(parser);
        if (status == FAILURE) {
            parser_report_error(parser);
            return FAILURE;
        }
        return status;
    }

    return SUCCESS;
}

internal u8
compile_logger_stage(compile_options *options, File *file, Lexer *lexer, Parser *parser)
{
    if (!options->debug_info) {
        return SUCCESS;
    }

    options->st = ST_LOGGER;
    FILE *output = fopen(OUTPUT_LOG_FILE, "wb");
    if (!output) {
        log_error("Failed to create log file: " OUTPUT_LOG_FILE);
        return FAILURE;
    }

    log_compilation(output, file, lexer, parser);
    fclose(output);
    return SUCCESS;
}

compile_info_stats
compile(compile_options *options)
{
    compile_info_stats exit_stats = {0};
    File file = {0};
    Lexer lexer = {0};
    Parser parser = {0};

    // Stage 1: File Reading
    if (compile_file_stage(options, &file) == FAILURE) {
        exit_stats.status = FAILURE;
        return exit_stats;
    }
    exit_stats.file_size = file.length;

    // Stage 2: Lexical Analysis
    if (compile_lexer_stage(options, &file, &lexer, &exit_stats) == FAILURE) {
        exit_stats.status = FAILURE;
        goto cleanup;
    }

    // Stage 3: Parsing
    if (compile_parser_stage(options, &lexer, &parser) == FAILURE) {
        exit_stats.status = FAILURE;
        goto cleanup;
    }

    // Stage 4: Logging (if requested)
    if (compile_logger_stage(options, &file, &lexer, &parser) == FAILURE) {
        exit_stats.status = FAILURE;
        goto cleanup;
    }

    exit_stats.status = SUCCESS;

cleanup:
    // Free resources
    file_free(&file);
    lexer_deinit(&lexer);
    parser_deinit(&parser);

    return exit_stats;
}

internal void
init_compile_options(compile_options *co, const i32 argc, i8 **argv)
{
    co->argc          = argc;
    co->argv          = argv;
    co->debug_info    = false;
    co->debug_symbols = false;
    co->timer         = false;
    co->lex_only      = false;
    co->st            = ST_UNKNOWN;
    co->filename      = argv[1];
}

internal void
parse_compile_argument(compile_options *co, cstr arg)
{
    if (strcmp(arg, "--log") == 0) {
        co->debug_info = true;
    }
    else if (!strcmp(arg, "--version") || !strcmp(arg, "-v")) {
        print_version_and_exit();
    }
    else if (strcmp(arg, "--timer") == 0) {
        co->timer = true;
    }
    else if (strcmp(arg, "--lex") == 0) {
        co->lex_only = true;
    }
    else {
        log_error_unknown_flag(arg);
    }
}

compile_options
compile_options_new(const i32 argc, i8 **argv)
{
    compile_options co;
    init_compile_options(&co, argc, argv);

    for (i32 i = 2; i < argc; i++) {
        parse_compile_argument(&co, argv[i]);
    }

    return co;
}

void
print_version_and_exit(void)
{
    cstr out = " Rotate Compiler \n Version: %s\n"
               " --lex   for lexical analysis\n"
               " --log   for dumping compilation info as orgmode format in output.org\n"
               " https://github.com/Airbus5717/rotate-c"
               "\n";
    fprintf(stdout, out, RTVERSION);
    exit(0);
}
