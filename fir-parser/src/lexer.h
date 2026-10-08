#ifndef FIR_LEXER_H
#define FIR_LEXER_H

#include <stddef.h>

typedef enum {
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COMMA,
    TOK_EQ,
    TOK_IDENT,
    TOK_STRING,
    TOK_NUMBER,
    TOK_BOOL,
    TOK_EOF
} TokenKind;

typedef struct {
    TokenKind kind;
    char *text;
    int line;
} Token;

typedef struct {
    const char *src;
    size_t pos;
    size_t len;
    int line;
} Lexer;

void lexer_init(Lexer *lx, const char *src);
Token lexer_next(Lexer *lx);
void token_free(Token *tok);

#endif
