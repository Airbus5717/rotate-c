#include "include/log.h"
#include "include/common.h"
#include "include/compile.h"
#include "include/file.h"

#include "fe/lexer.h"
#include "fe/parser.h"
#include "fe/token.h"

void
log_compilation(FILE *output, File *code_file, Lexer *lexer)
{
    time_t rawtime;
    time(&rawtime);
    assert(code_file && lexer);

    const Array(Token) tokens = lexer->tokens;
    if (array_length(tokens) > 0x100000)
    {
        log_warn("Too large file to show log");
        return;
    }
    log_warn("Logging will slow down compiliation ");
    fprintf(output, "#+TITLE: COMPILATION LOG" NEWLINE);
    fprintf(output, "#+OPTIONS: toc:nil num:nil" NEWLINE);
    fprintf(output, "#+AUTHOR: Rotate compiler" NEWLINE);
    fprintf(output, "#+DATE: %s" NEWLINE, asctime(localtime(&rawtime)));
    fprintf(output, "** Meta\n");
    fprintf(output, "- filename: =%s=" NEWLINE, code_file->name);
    fprintf(output, "- file length(chars): %u chars" NEWLINE, code_file->length);
    fprintf(output, "- time: %s", asctime(localtime(&rawtime)));
    fprintf(output, "- number of tokens: %llu" NEWLINE NEWLINE, tokens->count);
    fprintf(output, "** FILE" NEWLINE);
    fprintf(output, "#+begin_src cpp " NEWLINE "%s" NEWLINE "#+end_src" NEWLINE NEWLINE,
            code_file->contents);

    // TOKENS LOG STAGE
    fprintf(output, "** TOKENS" NEWLINE);
    fprintf(output, "#+begin_src" NEWLINE);
    uint i = 0;
    Token tkn;
    array_for_each(lexer->tokens, tkn)
    {
        fprintf(output, "[TOKEN]: n: %u, idx: %u, line: %u, len: %u, type: %s, val: `%.*s`" NEWLINE,
                i, tkn->index, tkn->line, tkn->length, tkn_type_describe(tkn->type), tkn->length,
                code_file->contents + tkn->index);
        i++;
    }
    fprintf(output, "#+end_src" NEWLINE);

    // PARSER STAGE
    // fprintf(output, "\n** Parser Abstract Syntax Tree" NEWLINE);
    // fprintf(output, "*** Imports " NEWLINE);
    // fprintf(output, "#+begin_src" NEWLINE);
    // for (uint i = 0; i < parser->ast->imports.size(); i++)
    // {
    //     const AstImport &m = parser->ast->imports[i];
    //     if (m.aliased)
    //         fprintf(output, "[IMPORT]: n: %u, import_str_idx: %u, alias_idx: %u" NEWLINE, i,
    //                 m.import_str, m.alias_id);
    //     else
    //         fprintf(output, "[IMPORT]: n: %u, id_idx: %u " NEWLINE, i, m.import_str);
    // }
    // fprintf(output, "#+end_src" NEWLINE);
    fprintf(output, NEWLINE "** TODO PARSER" NEWLINE);
    fprintf(output, NEWLINE "** TODO TYPECHECKER" NEWLINE);
    log_info("Logging complete");
}
