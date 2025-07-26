#include "include/log.h"
#include "include/common.h"
#include "include/compile.h"
#include "include/file.h"

#include "fe/lexer.h"
#include "fe/parser.h"
#include "fe/token.h"

void
log_compilation(FILE *output, File *code_file, Lexer *lexer, Parser *parser)
{
    time_t rawtime;
    time(&rawtime);
    assert(code_file && lexer && parser);

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
    fprintf(output, NEWLINE "** Parser Abstract Syntax Tree" NEWLINE);
    fprintf(output, "*** Declarations" NEWLINE);
    fprintf(output, "#+begin_src" NEWLINE);
    
    if (parser->ast && parser->ast->declarations) {
        for (usize i = 0; i < array_count(parser->ast->declarations); i++) {
            AstDecl *decl = array_at(parser->ast->declarations, i);
            if (!decl) continue;
            
            switch (decl->kind) {
                case AST_DECL_IMPORT:
                    if (decl->import.alias.length > 0) {
                        fprintf(output, "[IMPORT]: n: %zu, alias: `%.*s`, module: `%.*s`" NEWLINE, 
                               i, decl->import.alias.length, code_file->contents + decl->import.alias.index,
                               decl->import.module_path.length, code_file->contents + decl->import.module_path.index);
                    } else {
                        fprintf(output, "[IMPORT]: n: %zu, module: `%.*s`" NEWLINE, 
                               i, decl->import.module_path.length, code_file->contents + decl->import.module_path.index);
                    }
                    break;
                case AST_DECL_FUNCTION:
                    fprintf(output, "[FUNCTION]: n: %zu, name: `%.*s`, params: %zu" NEWLINE, 
                           i, decl->function.name.length, code_file->contents + decl->function.name.index,
                           array_count(decl->function.parameters));
                    break;
                case AST_DECL_VARIABLE:
                    fprintf(output, "[VARIABLE]: n: %zu, name: `%.*s`, constant: %s" NEWLINE, 
                           i, decl->variable.name.length, code_file->contents + decl->variable.name.index,
                           decl->variable.is_constant ? "yes" : "no");
                    break;
                case AST_DECL_STRUCT:
                    fprintf(output, "[STRUCT]: n: %zu, name: `%.*s`, fields: %zu" NEWLINE, 
                           i, decl->struct_decl.name.length, code_file->contents + decl->struct_decl.name.index,
                           array_count(decl->struct_decl.fields));
                    break;
                case AST_DECL_ENUM:
                    fprintf(output, "[ENUM]: n: %zu, name: `%.*s`, members: %zu" NEWLINE, 
                           i, decl->enum_decl.name.length, code_file->contents + decl->enum_decl.name.index,
                           array_count(decl->enum_decl.members));
                    break;
                default:
                    fprintf(output, "[UNKNOWN_DECL]: n: %zu, kind: %d" NEWLINE, i, decl->kind);
                    break;
            }
        }
    } else {
        fprintf(output, "No AST declarations found" NEWLINE);
    }
    fprintf(output, "#+end_src" NEWLINE);
    fprintf(output, NEWLINE "** TODO TYPECHECKER" NEWLINE);
    log_info("Logging complete");
}
