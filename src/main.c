#include "include/common.h"
#include "include/compile.h"

internal cstr
stage_to_string(Stage s)
{
    switch (s)
    {
        case ST_FILE: return "FILE READ";
        case ST_LEXER: return "LEXER";
        case ST_PARSER: return "PARSER";
        case ST_TCHECKER: return "TYPE CHECKER";
        case ST_LOGGER: return "LOGGER";
        default: return "UNKNOWN";
    }
}

internal void
print_compilation_stats(compile_info_stats stats, f128 total_time)
{
    printf("[%sINFO%s] : %d Tokens\n", LMAGENTA BOLD, RESET, stats.token_count);
    printf("[%sRATE%s] : %.3Lf mb/sec\n", LMAGENTA BOLD, RESET,
           (stats.file_size/(1024*1024))/total_time);
    printf("[%sTIME%s] : %.5Lf sec\n", LMAGENTA BOLD, RESET, total_time);
}

int
main(const int argc, char **const argv)
{
    if (argc < 2)
    {
        print_version_and_exit();
        return SUCCESS;
    }

    // parse program arguments
    compile_options comp_opt = compile_options_new(argc, argv);

    // setup timer
    clock_t start_time = clock();
    
    // compile
    compile_info_stats exit_stats = compile(&comp_opt);

    // handle compilation results
    if (exit_stats.status == FAILURE) {
        log_stage(stage_to_string(comp_opt.st));
        fprintf(stderr, "Compilation failed at stage: %s\n", stage_to_string(comp_opt.st));
        return FAILURE;
    } else if (exit_stats.status == SUCCESS) {
        log_info("Compilation succeeded.");
    }

    // print compilation statistics
    if (comp_opt.timer) {
        clock_t end_time = clock();
        f128 total_time = (f64)(end_time - start_time) / CLOCKS_PER_SEC;
        print_compilation_stats(exit_stats, total_time);
    }
    return SUCCESS;
}
