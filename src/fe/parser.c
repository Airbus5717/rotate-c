#include "parser.h"
#include "token.h"
#include "type.h"

/*
 *
 * Internal private functions definitions
 *
 */
internal u8 parse_director(Parser *);
internal u8 parse_imports(Parser *);
internal u8 parse_functions(Parser *);
internal u8 parse_structs(Parser *);
internal u8 parse_enums(Parser *);
internal u8 parse_gl_vars(Parser *);
internal u8 parse_code_block(Parser *);

// NOTE(5717): useful parser utils
inline internal Token
current(Parser *p)
{
    return array_at(p->lexer->tokens, (p->index));
}

inline internal Token
next(Parser *p)
{
    return array_at(p->lexer->tokens, (p->index + 1));
}

inline internal Token
previous(Parser *p)
{
    return array_at(p->lexer->tokens, (p->index - 1));
}

inline internal void
advance(Parser *p)
{
    p->index++;
}

inline internal u8
expect_n_consume(Parser *p, TknType t)
{
    Token tkn = current(p);
    if (tkn.type == t)
    {
        advance(p);
        return SUCCESS;
    }
    return FAILURE;
}
/*
 *
 * Public functions
 *
 */
Parser
parser_init(Lexer *l)
{
    Parser parser = {0};
    parser.lexer  = l;
    parser.index  = 0;
    // TODO: allocate ast
    return parser;
}

u8
parser_parse(Parser *p)
{
    UNUSED(p);
    TODO("implement parser");
    return FAILURE;
}

void
parser_deinit(Parser *p)
{
    UNUSED(p);
    TODO("implement deinitializer");
}

/*
 *
 * internal functions
 *
 */
u8
parse_director(Parser *p)
{
    u8 exit = SUCCESS;
    while (exit == SUCCESS)
    {

        switch (current(p).type)
        {
            case Tkn_ImportKeyword: exit = parse_imports(p); break;
            case Tkn_FnKeyword: exit = parse_functions(p); break;
            case Tkn_StructKeyword: exit = parse_structs(p); break;
            case Tkn_EnumKeyword: exit = parse_enums(p); break;
            case Tkn_LetKeyword: exit = parse_gl_vars(p); break;
            case Tkn_EOT: exit = DONE; break;
            default: break;
        }
    }
    return exit;
}

u8
parse_imports(Parser *p)
{
    advance(p); // skip 'import'
    TODO("implement parser imports");
    return FAILURE;
}

u8
parse_functions(Parser *p)
{
    advance(p); // skip 'fn'
    TODO("implement parse functions");
    return FAILURE;
}

u8
parse_structs(Parser *p)
{
    advance(p); // skip 'struct'
    TODO("implement parse structs");
    return FAILURE;
}

u8
parse_enums(Parser *p)
{
    advance(p); // skip 'enum'
    TODO("implement parse enums");
    return FAILURE;
}

u8
parse_gl_vars(Parser *p)
{
    advance(p); // skip 'let'
    TODO("implement parse global variables");
    return FAILURE;
}

u8
parse_code_block(Parser *p)
{
    advance(p);
    TODO("implement parse code blocks");
    return FAILURE;
}

u8
parse_if_statment(Parser *p)
{
    advance(p);
    TODO("implement parse if statement");
    return FAILURE;
}
