#include "include/log.h"
#include "include/common.h"
#include "include/compile.h"
#include "include/file.h"

#include "fe/lexer.h"
#include "fe/parser.h"
#include "fe/token.h"

#define MAX_LOG_TOKENS 0x100000
#define ORGMODE_NEWLINE NEWLINE

internal void
log_header(FILE *output, time_t rawtime)
{
    fprintf(output, "#+TITLE: COMPILATION LOG" ORGMODE_NEWLINE);
    fprintf(output, "#+OPTIONS: toc:nil num:nil" ORGMODE_NEWLINE);
    fprintf(output, "#+AUTHOR: Rotate compiler" ORGMODE_NEWLINE);
    fprintf(output, "#+DATE: %s" ORGMODE_NEWLINE, asctime(localtime(&rawtime)));
}

internal void
log_metadata(FILE *output, File *code_file, time_t rawtime, usize token_count)
{
    fprintf(output, "** Meta\n");
    fprintf(output, "- filename: =%s=" ORGMODE_NEWLINE, code_file->name);
    fprintf(output, "- file length(chars): %u chars" ORGMODE_NEWLINE, code_file->length);
    fprintf(output, "- time: %s", asctime(localtime(&rawtime)));
    fprintf(output, "- number of tokens: %llu" ORGMODE_NEWLINE ORGMODE_NEWLINE, token_count);
}

internal void
log_source_file(FILE *output, File *code_file)
{
    fprintf(output, "** FILE" ORGMODE_NEWLINE);
    fprintf(output, "#+begin_src cpp " ORGMODE_NEWLINE "%s" ORGMODE_NEWLINE "#+end_src" ORGMODE_NEWLINE ORGMODE_NEWLINE,
            code_file->contents);
}

internal void
log_tokens(FILE *output, File *code_file, Lexer *lexer)
{
    fprintf(output, "** TOKENS" ORGMODE_NEWLINE);
    fprintf(output, "#+begin_src" ORGMODE_NEWLINE);
    
    uint i = 0;
    Token tkn;
    array_for_each(lexer->tokens, tkn)
    {
        fprintf(output, "[TOKEN]: n: %u, idx: %u, line: %u, len: %u, type: %s, val: `%.*s`" ORGMODE_NEWLINE,
                i, tkn->index, tkn->line, tkn->length, tkn_type_describe(tkn->type), tkn->length,
                code_file->contents + tkn->index);
        i++;
    }
    fprintf(output, "#+end_src" ORGMODE_NEWLINE);
}

internal void
log_declaration(FILE *output, File *code_file, AstDecl *decl, usize index)
{
    switch (decl->kind) {
        case AST_DECL_IMPORT:
            if (decl->import.alias.length > 0) {
                fprintf(output, "[IMPORT]: n: %llu, alias: `%.*s`, module: `%.*s`" ORGMODE_NEWLINE, 
                       index, (int)decl->import.alias.length, code_file->contents + decl->import.alias.index,
                       (int)decl->import.module_path.length, code_file->contents + decl->import.module_path.index);
            } else {
                fprintf(output, "[IMPORT]: n: %llu, module: `%.*s`" ORGMODE_NEWLINE, 
                       index, (int)decl->import.module_path.length, code_file->contents + decl->import.module_path.index);
            }
            break;
        case AST_DECL_FUNCTION:
            fprintf(output, "[FUNCTION]: n: %llu, name: `%.*s`, params: %llu" ORGMODE_NEWLINE, 
                   index, (int)decl->function.name.length, code_file->contents + decl->function.name.index,
                   array_count(decl->function.parameters));
            break;
        case AST_DECL_VARIABLE:
            fprintf(output, "[VARIABLE]: n: %llu, name: `%.*s`, constant: %s" ORGMODE_NEWLINE, 
                   index, (int)decl->variable.name.length, code_file->contents + decl->variable.name.index,
                   decl->variable.is_constant ? "yes" : "no");
            break;
        case AST_DECL_STRUCT:
            fprintf(output, "[STRUCT]: n: %llu, name: `%.*s`, fields: %llu" ORGMODE_NEWLINE, 
                   index, (int)decl->struct_decl.name.length, code_file->contents + decl->struct_decl.name.index,
                   array_count(decl->struct_decl.fields));
            break;
        case AST_DECL_ENUM:
            fprintf(output, "[ENUM]: n: %llu, name: `%.*s`, members: %llu" ORGMODE_NEWLINE, 
                   index, (int)decl->enum_decl.name.length, code_file->contents + decl->enum_decl.name.index,
                   array_count(decl->enum_decl.members));
            break;
        default:
            fprintf(output, "[UNKNOWN_DECL]: n: %llu, kind: %d" ORGMODE_NEWLINE, index, decl->kind);
            break;
    }
}

internal void
log_ast(FILE *output, File *code_file, Parser *parser)
{
    fprintf(output, ORGMODE_NEWLINE "** Parser Abstract Syntax Tree" ORGMODE_NEWLINE);
    fprintf(output, "*** Declarations" ORGMODE_NEWLINE);
    fprintf(output, "#+begin_src" ORGMODE_NEWLINE);
    
    if (parser->ast && parser->ast->declarations) {
        for (usize i = 0; i < array_count(parser->ast->declarations); i++) {
            AstDecl *decl = array_at(parser->ast->declarations, i);
            if (!decl) continue;
            log_declaration(output, code_file, decl, i);
        }
    } else {
        fprintf(output, "No AST declarations found" ORGMODE_NEWLINE);
    }
    fprintf(output, "#+end_src" ORGMODE_NEWLINE);
    fprintf(output, ORGMODE_NEWLINE "** TODO TYPECHECKER" ORGMODE_NEWLINE);
}

void
log_compilation(FILE *output, File *code_file, Lexer *lexer, Parser *parser)
{
    time_t rawtime;
    time(&rawtime);
    assert(code_file && lexer && parser);

    const Array(Token) tokens = lexer->tokens;
    if (array_length(tokens) > MAX_LOG_TOKENS)
    {
        log_warn("File too large to show complete log");
        return;
    }
    
    log_warn("Logging will slow down compilation");
    
    log_header(output, rawtime);
    log_metadata(output, code_file, rawtime, tokens->count);
    log_source_file(output, code_file);
    log_tokens(output, code_file, lexer);
    log_ast(output, code_file, parser);
    
    log_info("Logging complete");
}
