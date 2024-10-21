#include "lexer.h"
#include "token.h"

// PRIVATE LEXER METHODS
internal u8 lex_director(Lexer *);
internal u8 lex_chars(Lexer *);
internal u8 lex_numbers(Lexer *);
internal u8 lex_strings(Lexer *);
// internal u8 lex_multiline_strings(Lexer *);
internal u8 lex_binary_numbers(Lexer *);
internal u8 lex_hex_numbers(Lexer *);
internal u8 lex_symbols(Lexer *);
internal u8 lex_builtin_funcs(Lexer *);
internal u8 lex_identifiers(Lexer *);
internal void lex_save_state(Lexer *);
internal void lex_restore_state_for_err(Lexer *);
internal u8 lex_report_error(Lexer *l);
internal u8 lex_add_token(Lexer *, TknType);
internal void lex_advance(Lexer *);
internal void lex_advance_len_inc(Lexer *);
internal void lex_advance_len_times(Lexer *);
internal char lex_peek(Lexer *);
internal char lex_past(Lexer *);
internal char lex_current(Lexer *);
internal bool lex_is_not_eof(Lexer *);
internal void lex_skip_whitespace(Lexer *);
internal bool lex_keyword_match(Lexer *, cstr, uint);
internal u8 lex_add_terminator(Lexer *);

// Lexer API
// file must not be null and l owns the file ptr
Lexer
lexer_init(File *file)
{
    ASSERT_NULL(file, "Lexer File passed is a null pointer");
    Lexer l          = {0};
    l.index          = 0;
    l.len            = 0;
    l.line           = 1;
    l.file_length    = file ? file->length : 0;
    l.file           = file;
    l.error          = LE_UNKNOWN;
    l.begin_tkn_line = 1;
    l.save_line      = 1;
    l.save_index     = 0;
    l.tokens         = make_array(Token, file->length / 4);
    l.prev           = Tkn_EOT;

    ASSERT_NULL(l.tokens, "Lexer vec of tokens passed is a null pointer");
    return l;
}

void
lexer_deinit(Lexer *l)
{
    array_free(l->tokens);
}

void
lexer_save_log(Lexer *l, FILE *output)
{
    array_foreach(l->tokens, el)
    {
        log_token(output, *el, l->file->contents);
    }
}

u8
lexer_lex(Lexer *l)
{
    for (;;)
    {
        u8 status = lex_director(l);
        switch (status)
        {
            case SUCCESS: break;
            case DONE: {
                l->len = 0;
                for (u8 i = 0; i < EXTRA_NULL_TERMINATORS; ++i)
                    lex_add_token(l, Tkn_EOT);
                return SUCCESS;
            }
            case FAILURE: return lex_report_error(l);
        }
    }
    return FAILURE;
}

ArrayList(Token) lexer_get_tokens(Lexer *l)
{
    return l->tokens;
}

// PRIVATE internals

inline void
lex_skip_whitespace(Lexer *l)
{
    for (;;)
    {
        const char c = lex_current(l);
        if (c == ' ' || c == '\t' || c == '\r')
        {
            l->index += 1;
        }
        else if (c == '\n')
        {
            lex_add_terminator(l);
            l->line += 1;
        }
        else
        {
            break;
        }
    }
}

u8
lex_director(Lexer *l)
{
    l->len = 0, l->begin_tkn_line = l->line;
    lex_skip_whitespace(l);
    lex_save_state(l);
    const char c = lex_current(l);
    // ints and floats
    if (isdigit(c)) return lex_numbers(l);
    // chars, and strings
    // TODO: Multiline strings
    if (c == '"') return lex_strings(l);
    if (c == '\'') return lex_chars(l);

    // NOTE: Idenitifiers, keywords and builtin functions
    if (c == '_' || isalpha(c)) return lex_identifiers(l);
    if (c == '@') return lex_builtin_funcs(l);
    //  Symbols
    return lex_symbols(l);
}

u8
lex_identifiers(Lexer *l)
{
    lex_advance_len_inc(l);
    while (isalnum(lex_current(l)) || lex_current(l) == '_')
    {
        lex_advance_len_inc(l);
    }
    l->index -= l->len;
    TknType _type = Tkn_Identifier;

    // TODO: optimize searching for matching keywords
    switch (l->len)
    {
        case 2: {
            switch (lex_current(l))
            {
                case 'a':
                    if (lex_keyword_match(l, "as", 2)) _type = Tkn_AsKeyword;
                    break;
                case 'f':
                    if (lex_keyword_match(l, "fn", 2)) _type = Tkn_FnKeyword;
                    break;
                case 'i':
                    if (lex_keyword_match(l, "if", 2)) _type = Tkn_IfKeyword;
                    break;
                case 'o':
                    if (lex_keyword_match(l, "or", 2)) _type = Tkn_OrKeyword;
                    break;
                default: break;
            }

            break;
        }
        case 3: {
            switch (lex_current(l))
            {
                case 'l':
                    if (lex_keyword_match(l, "let", 3)) _type = Tkn_LetKeyword;
                    break;
                case 'f':
                    if (lex_keyword_match(l, "for", 3)) _type = Tkn_ForKeyword;
                    break;
                case 'p':
                    if (lex_keyword_match(l, "pub", 3)) _type = Tkn_PubKeyword;
                    break;
                case 'i':
                    if (lex_keyword_match(l, "int", 3)) _type = Tkn_IntKeyword;
                    break;
                case 'r':
                    if (lex_keyword_match(l, "ref", 3))
                        _type = Tkn_RefKeyword;
                    else if (lex_keyword_match(l, "ret", 3))
                        _type = Tkn_RetKeyword;
                    break;
                case 'a':
                    if (lex_keyword_match(l, "and", 3)) _type = Tkn_AndKeyword;
                    break;
                case 'n':
                    if (lex_keyword_match(l, "nil", 3)) _type = Tkn_NilLiteral;
                    break;
            }
            break;
        }
        case 4: {
            switch (lex_current(l))
            {
                case 'e': {
                    if (lex_keyword_match(l, "else", 4))
                        _type = Tkn_ElseKeyword;
                    else if (lex_keyword_match(l, "enum", 4))
                        _type = Tkn_EnumKeyword;
                    break;
                }
                case 't':
                    if (lex_keyword_match(l, "true", 4)) _type = Tkn_TrueLiteral;
                    break;
                case 'c':
                    if (lex_keyword_match(l, "char", 4)) _type = Tkn_CharKeyword;
                    break;
                case 'b':
                    if (lex_keyword_match(l, "bool", 4)) _type = Tkn_BoolKeyword;
                    break;
                case 'u':
                    if (lex_keyword_match(l, "uint", 4)) _type = Tkn_UIntKeyword;
                    break;
                case 'f':
                    if (lex_keyword_match(l, "fall", 4)) _type = Tkn_FallKeyword;
                    break;
            }
            break;
        }
        case 5: {
            switch (lex_current(l))
            {
                case 'w':
                    if (lex_keyword_match(l, "while", 5)) _type = Tkn_WhileKeyword;
                    break;
                case 'f': {
                    if (lex_keyword_match(l, "false", 5))
                        _type = Tkn_FalseLiteral;
                    else if (lex_keyword_match(l, "float", 5))
                        _type = Tkn_FltKeyword;
                    break;
                }
                case 'b':
                    if (lex_keyword_match(l, "break", 5)) _type = Tkn_BreakKeyword;
                    break;
            }
            break;
        }
        case 6: {
            switch (lex_current(l))
            {
                case 'i':
                    if (lex_keyword_match(l, "import", 6)) _type = Tkn_ImportKeyword;
                    break;
                case 'd':
                    if (lex_keyword_match(l, "delete", 6)) _type = Tkn_DeleteKeyword;
                    break;
                case 's': {
                    if (lex_keyword_match(l, "struct", 6))
                        _type = Tkn_StructKeyword;
                    else if (lex_keyword_match(l, "switch", 6))
                        _type = Tkn_SwitchKeyword;
                    break;
                }
            }
            break;
        }
        default: break;
    }

    if (l->len > 100)
    {
        // log_error("identifier length is more than 128 chars");
        l->error = LE_TOO_LONG_IDENTIFIER;
        lex_restore_state_for_err(l);
        return FAILURE;
    }

    return lex_add_token(l, _type);
}

u8
lex_numbers(Lexer *l)
{
    const char c = lex_current(l);
    const char p = lex_peek(l);

    // 0x or 0b switch state of lexing to specific radix
    // x -> hexadecimal(r16) | b -> binary(r2)
    if (c == '0')
    {
        switch (p)
        {
            case 'x': return lex_hex_numbers(l);
            case 'b': return lex_binary_numbers(l);
            default: break;
        }
    }

    bool reached_dot = false;
    while (isdigit(lex_current(l)) || lex_current(l) == '.')
    {
        lex_advance_len_inc(l);
        if (lex_current(l) == '.')
        {
            if (reached_dot) break;
            reached_dot = true;
        }
    }

    if (l->len > 100)
    {
        log_error("number digits length is above 100");
        lex_restore_state_for_err(l);
        return FAILURE;
    }
    l->index -= l->len;
    return lex_add_token(l, reached_dot ? Tkn_FloatLiteral : Tkn_IntegerLiteral);
}

u8
lex_hex_numbers(Lexer *l)
{
    // skip '0x'
    lex_advance_len_inc(l);
    lex_advance_len_inc(l);
    while (isxdigit(lex_current(l)))
    {
        lex_advance_len_inc(l);
    }

    if (l->len > 0x20)
    {
        log_error("hex number digits length is above 32");
        lex_restore_state_for_err(l);
        return FAILURE;
    }
    l->index -= l->len;
    return lex_add_token(l, Tkn_IntegerLiteral);
}

u8
lex_binary_numbers(Lexer *l)
{
    // skip '0b'
    lex_advance_len_inc(l);
    lex_advance_len_inc(l);
    while (lex_current(l) == '0' || lex_current(l) == '1')
    {
        lex_advance_len_inc(l);
    }

    if (l->len > 0x80)
    {
        log_error("binary number digits length is above 128");
        lex_restore_state_for_err(l);
        return FAILURE;
    }
    l->index -= l->len;
    return lex_add_token(l, Tkn_IntegerLiteral);
}

u8
lex_strings(Lexer *l)
{
    lex_advance_len_inc(l);
    for (;;)
    {
        if (lex_current(l) == '\0')
        {
            lex_restore_state_for_err(l);
            l->error = LE_NOT_CLOSED_STRING;
            return FAILURE;
        }

        if (lex_current(l) == '"')
        {
            if (lex_past(l) == '\\')
            {
                lex_advance_len_inc(l);
                continue;
            }
            else
                break;
        }
        lex_advance_len_inc(l);
    }
    lex_advance_len_inc(l);

    if (l->len > (RUINT_MAX / 2))
    {
        lex_restore_state_for_err(l);
        log_error("A cstr is not allowed to be longer than (Uuint_MAX / 2)");
        l->error = LE_TOO_LONG_STRING;
        return FAILURE;
    }
    l->index -= l->len;
    return lex_add_token(l, Tkn_StringLiteral);
}

u8
lex_chars(Lexer *l)
{
    lex_advance_len_inc(l);
    if (lex_current(l) != '\\' && lex_peek(l) == '\'')
    {
        lex_advance_len_inc(l);
        lex_advance_len_inc(l);
        l->index -= l->len;
        return lex_add_token(l, Tkn_CharLiteral);
    }
    else if (lex_current(l) == '\\')
    {
        lex_advance_len_inc(l);
        switch (lex_current(l))
        {
            case 'n':
            case 't':
            case 'r':
            case '\\':
            case '\'': lex_advance_len_inc(l); break;
            default:
                l->error = LE_NOT_VALID_ESCAPE_CHAR;
                lex_restore_state_for_err(l);
                return FAILURE;
        }
        if (lex_current(l) == '\'')
        {
            lex_advance_len_inc(l);
            l->index -= l->len;
            return lex_add_token(l, Tkn_CharLiteral);
        }
        else
        {
            l->error = LE_LEXER_INVALID_CHAR;
            lex_restore_state_for_err(l);
            return FAILURE;
        }
    }

    return FAILURE;
}

u8
lex_symbols(Lexer *l)
{
    const char c = lex_current(l);
    const char p = lex_peek(l);
    l->len       = 1;
    switch (c)
    {
        case '{': return lex_add_token(l, Tkn_OpenCurly);
        case '}': return lex_add_token(l, Tkn_CloseCurly);
        case '(': return lex_add_token(l, Tkn_OpenParen);
        case ')': return lex_add_token(l, Tkn_CloseParen);
        case '[': return lex_add_token(l, Tkn_OpenSQRBrackets);
        case ']': return lex_add_token(l, Tkn_CloseSQRBrackets);
        case ',': return lex_add_token(l, Tkn_Comma);
        case ';': return lex_add_terminator(l);
        case '.': return lex_add_token(l, Tkn_Dot);
        case '%': return lex_add_token(l, Tkn_Mod);
        // TODO(5717) bug below needs to check an eql during peeking

        // more than one length char
        case ':': {
            return lex_add_token(l, Tkn_Colon);
        }
        case '>': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_GreaterEql);
            }
            else if (p == '>')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_RightShift);
            }
            return lex_add_token(l, Tkn_Greater);
        }
        case '<': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_LessEql);
            }
            else if (p == '<')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_LeftShift);
            }
            return lex_add_token(l, Tkn_Less);
        }
        case '=': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_EqualEqual);
            }
            return lex_add_token(l, Tkn_Equal);
        }
        case '+': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_AddEqual);
            }
            return lex_add_token(l, Tkn_PlusOperator);
        }
        case '-': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_SubEqual);
            }
            return lex_add_token(l, Tkn_MinusOperator);
        }
        case '*': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_MultEqual);
            }
            return lex_add_token(l, Tkn_MultOperator);
        }
        case '/': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_DivEqual);
            }
            else if (p == '/')
            {
                //
                while (lex_is_not_eof(l) && lex_current(l) != '\n')
                    lex_advance(l);
                return SUCCESS;
            }
            else if (p == '*')
            {
                lex_advance(l); // skip '/'
                lex_advance(l); // skip '*'

                // TODO: Allow nested comments
                bool end_comment = false;
                while (lex_is_not_eof(l) && !end_comment)
                {
                    if (lex_current(l) == '*' && lex_peek(l) == '/')
                    {
                        lex_advance(l);
                        end_comment = true;
                    }
                    lex_advance(l);
                }
                return SUCCESS;
            }
            return lex_add_token(l, Tkn_DivOperator);
        }
        case '!': {
            if (p == '=')
            {
                l->len += 1;
                return lex_add_token(l, Tkn_NotEqual);
            }
            return lex_add_token(l, Tkn_Not);
        }
        case '#': {
            // this is for comments
            while (lex_current(l) != '\n' && lex_is_not_eof(l))
                lex_advance(l);
            lex_advance(l);
            return SUCCESS;
        }
        default: {
            switch (c)
            {
                case '\0': return DONE;
                default: {
                    l->error = LE_LEXER_INVALID_CHAR;
                }
            }
        }
    }
    return FAILURE;
}

u8
lex_builtin_funcs(Lexer *l)
{
    lex_advance(l); // skip '@'

    // NOTE(5717): ONLY ALPHABET CHARS ARE ALLOWED
    while (isalpha(lex_current(l)))
    {
        lex_advance_len_inc(l);
    }
    l->index -= l->len;
    // NOTE(5717): check if needed to specify the type
    return lex_add_token(l, Tkn_BuiltinId);
}

inline void
lex_advance(Lexer *l)
{
    const char c = lex_current(l);
    l->index += 1;
    l->line += (c == '\n');
}

inline void
lex_advance_len_times(Lexer *l)
{
    l->index += l->len;
}

inline void
lex_advance_len_inc(Lexer *l)
{
    const char c = lex_current(l);
    l->index += 1;
    l->len += 1;
    l->line += (c == '\n');
}

inline char
lex_peek(Lexer *l)
{
    return l->file->contents[l->index + 1];
}

inline char
lex_current(Lexer *l)
{
    return l->file->contents[l->index];
}

inline char
lex_past(Lexer *l)
{
    return l->file->contents[l->index - 1];
}

inline bool
lex_is_not_eof(Lexer *l)
{
    return l->index < l->file_length;
}

inline bool
lex_keyword_match(Lexer *l, cstr cstr, uint length)
{
    bool res = (strncmp(l->file->contents + l->index, cstr, length) == 0);
    return res;
}

inline void
lex_save_state(Lexer *l)
{
    l->save_index = l->index;
    l->save_line  = l->line;
}

inline void
lex_restore_state_for_err(Lexer *l)
{
    l->index = l->save_index;
    l->line  = l->save_line;
}

u8
lex_report_error(Lexer *l)
{
    //
    uint low = l->index, col = 0;
    const uint line  = l->line;
    const uint len   = l->len;
    const uint index = l->index;
    File *file       = l->file;
    while (file->contents[low] != '\n' && low > 0)
    {
        low--;
        col++;
    }
    low = low > 1 ? low + 1 : 0;

    //
    uint _length = l->index;
    while (file->contents[_length] != '\n' && _length + 1 < file->length)
        _length++;

    _length -= low;

    // error msg
    fprintf(stderr, " > %s%s%s:%u:%u: %serror: %s%s%s\n", BOLD, WHITE, file->name, line, col, LRED,
            LBLUE, lexer_err_msg(l->error), RESET);

    // line from source code
    fprintf(stderr, "  %s%u%s | %.*s\n", LYELLOW, line, RESET, _length, (file->contents + low));

    const uint num_line_digits = get_digits_from_number(line);

    // arrows pointing to error location
    const uint spaces = index - low + 1;

    const uint MAX_ARROW_LEN = 101;
    if (len < 101)
    {
        char arrows[MAX_ARROW_LEN];
        memset(arrows, 0, MAX_ARROW_LEN);
        memset(arrows, '^', len);
        arrows[len] = '\0';

        fprintf(stderr, "  %*c |%*c%s%s%s\n", num_line_digits, ' ', spaces, ' ', LRED, BOLD,
                arrows);
    }
    else
    {
        fprintf(stderr, "  %*c |%*c%s%s^^^---...\n", num_line_digits, ' ', spaces, ' ', LRED, BOLD);
    }
    // error lexer_err_advice
    fprintf(stderr, " > Advice: %s%s\n", RESET, lexer_err_advice(l->error));
    return FAILURE;
}

inline u8
lex_add_token(Lexer *l, TknType type)
{
    // index at the end of the token
    Token tkn = (Token){l->index, l->len, l->begin_tkn_line, type};
    array_push(l->tokens, tkn);
    l->prev = Tkn_Terminator;
    lex_advance_len_times(l);
    return SUCCESS;
}

inline u8
lex_add_terminator(Lexer *l)
{
    if (l->prev != Tkn_Terminator) return lex_add_token(l, Tkn_Terminator);

    lex_advance(l);
    return SUCCESS;
}
