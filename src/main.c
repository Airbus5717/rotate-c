#include "include/common.h"
#include "include/compile.h"

cstr
main_err(Stage s)
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
    return "UNKNOWN";
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

    // setup timer stuff
    clock_t start_t, end_t;
    f128 total_t;
    start_t = clock();

    // compile
    usize file_size = 0;
    compile_info_stats _exit = compile(&comp_opt);

    // Compile the input file and handle errors
    if (_exit.status == FAILURE) {
        log_stage(main_err(comp_opt.st));
        fprintf(stderr, "Compilation failed at stage: %s\n", main_err(comp_opt.st));
        return FAILURE;
    } else if (_exit.status == SUCCESS) {
        log_info("Compilation succeeded.");
    }

    // print comptime
    end_t   = clock();
    total_t = (f64)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("[%sINFO%s] : %d Tokens\n", LMAGENTA BOLD, RESET, _exit.token_count);
    printf("[%sRATE%s] : %.3Lf mb/sec\n", LMAGENTA BOLD, RESET,
		    (_exit.file_size/(1024*1024))/total_t);
    printf("[%sTIME%s] : %.5Lf sec\n", LMAGENTA BOLD, RESET, total_t);
    return SUCCESS;
}
