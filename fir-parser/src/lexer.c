#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

void lexer_init(Lexer *lx, const char *src) {
    lx->src = src;
    lx->pos = 0;
    lx->len = strlen(src);
    lx->line = 1;
}

void token_free(Token *tok) {
    free(tok->text);
    tok->text = NULL;
}

static int peek(const Lexer *lx) {
    return lx->pos < lx->len ? (unsigned char)lx->src[lx->pos] : 0;
}

static int peek_at(const Lexer *lx, size_t off) {
    return lx->pos + off < lx->len ? (unsigned char)lx->src[lx->pos + off] : 0;
}

static int bump(Lexer *lx) {
    int c = peek(lx);
    lx->pos++;
    if (c == '\n')
        lx->line++;
    return c;
}

static void skip_trivia(Lexer *lx) {
    for (;;) {
        int c = peek(lx);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            bump(lx);
            continue;
        }
        if (c == '/' && peek_at(lx, 1) == '/') {
            while (lx->pos < lx->len && peek(lx) != '\n')
                lx->pos++;
            continue;
        }
        break;
    }
}

static Token make_token(Lexer *lx, TokenKind kind, const char *text) {
    Token tok;
    tok.kind = kind;
    tok.text = strdup(text);
    tok.line = lx->line;
    return tok;
}

static Token single_char(Lexer *lx, TokenKind kind, const char *text) {
    Token tok;
    bump(lx);
    tok.kind = kind;
    tok.text = strdup(text);
    tok.line = lx->line;
    return tok;
}

static Token read_string(Lexer *lx) {
    int line = lx->line;
    bump(lx);
    size_t cap = 16, len = 0;
    char *buf = malloc(cap);
    while (peek(lx) && peek(lx) != '"') {
        int c = bump(lx);
        if (c == '\\') {
            int e = bump(lx);
            switch (e) {
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case 'r': c = '\r'; break;
            case '"': c = '"'; break;
            case '\\': c = '\\'; break;
            default: c = e; break;
            }
        }
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = (char)c;
    }
    if (peek(lx) != '"') {
        fprintf(stderr, "parse error (line %d): unterminated string\n", line);
        exit(1);
    }
    bump(lx);
    buf[len] = '\0';

    Token tok;
    tok.kind = TOK_STRING;
    tok.text = buf;
    tok.line = line;
    return tok;
}

static Token read_word(Lexer *lx) {
    int line = lx->line;
    size_t start = lx->pos;
    while (isalnum((unsigned char)peek(lx)) || peek(lx) == '_' || peek(lx) == '-' || peek(lx) == '.')
        bump(lx);

    size_t len = lx->pos - start;
    char *text = malloc(len + 1);
    memcpy(text, lx->src + start, len);
    text[len] = '\0';

    Token tok;
    tok.text = text;
    tok.line = line;
    if (strcmp(text, "true") == 0 || strcmp(text, "false") == 0)
        tok.kind = TOK_BOOL;
    else
        tok.kind = TOK_IDENT;
    return tok;
}

static Token read_number(Lexer *lx) {
    int line = lx->line;
    size_t start = lx->pos;
    if (peek(lx) == '-')
        bump(lx);
    while (isdigit((unsigned char)peek(lx)) || peek(lx) == '.')
        bump(lx);

    size_t len = lx->pos - start;
    char *text = malloc(len + 1);
    memcpy(text, lx->src + start, len);
    text[len] = '\0';

    Token tok;
    tok.kind = TOK_NUMBER;
    tok.text = text;
    tok.line = line;
    return tok;
}

Token lexer_next(Lexer *lx) {
    skip_trivia(lx);

    int c = peek(lx);
    switch (c) {
    case 0: return make_token(lx, TOK_EOF, "");
    case '{': return single_char(lx, TOK_LBRACE, "{");
    case '}': return single_char(lx, TOK_RBRACE, "}");
    case '[': return single_char(lx, TOK_LBRACKET, "[");
    case ']': return single_char(lx, TOK_RBRACKET, "]");
    case ',': return single_char(lx, TOK_COMMA, ",");
    case '=': return single_char(lx, TOK_EQ, "=");
    case '"': return read_string(lx);
    default: break;
    }

    if (isdigit(c) || (c == '-' && isdigit((unsigned char)peek_at(lx, 1))))
        return read_number(lx);
    if (isalpha(c) || c == '_')
        return read_word(lx);

    fprintf(stderr, "parse error (line %d): unexpected character '%c'\n", lx->line, c);
    exit(1);
}
