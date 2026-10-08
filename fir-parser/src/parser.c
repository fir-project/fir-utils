#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

typedef struct {
    Lexer lx;
    Token tok;
} Parser;

static void fail(const Parser *p, const char *msg) {
    fprintf(stderr, "parse error (line %d): %s\n", p->tok.line, msg);
    exit(1);
}

static void advance(Parser *p) {
    token_free(&p->tok);
    p->tok = lexer_next(&p->lx);
}

static void expect(Parser *p, TokenKind kind, const char *msg) {
    if (p->tok.kind != kind)
        fail(p, msg);
}

static char *take_text(Parser *p) {
    char *text = p->tok.text;
    p->tok.text = NULL;
    return text;
}

static FirNode *parse_value(Parser *p);
static FirNode *parse_map(Parser *p);

static FirNode *parse_map_body(Parser *p, TokenKind end) {
    FirNode *map = fir_node_new(FIR_MAP);

    while (p->tok.kind != end) {
        expect(p, TOK_IDENT, "expected key");
        char *key = take_text(p);
        advance(p);

        FirNode *value;
        if (p->tok.kind == TOK_LBRACE) {
            value = parse_map(p);
        } else {
            expect(p, TOK_EQ, "expected '=' or '{' after key");
            advance(p);
            value = parse_value(p);
        }
        fir_map_add(map, key, value);
    }

    return map;
}

static FirNode *parse_map(Parser *p) {
    expect(p, TOK_LBRACE, "expected '{'");
    advance(p);

    FirNode *map = parse_map_body(p, TOK_RBRACE);

    advance(p);
    return map;
}

static FirNode *parse_list(Parser *p) {
    FirNode *list = fir_node_new(FIR_LIST);
    expect(p, TOK_LBRACKET, "expected '['");
    advance(p);

    while (p->tok.kind != TOK_RBRACKET) {
        fir_list_add(list, parse_value(p));
        if (p->tok.kind == TOK_COMMA)
            advance(p);
    }

    advance(p);
    return list;
}

static FirNode *parse_value(Parser *p) {
    switch (p->tok.kind) {
    case TOK_STRING: {
        FirNode *node = fir_node_new(FIR_STRING);
        node->string = take_text(p);
        advance(p);
        return node;
    }
    case TOK_BOOL: {
        FirNode *node = fir_node_new(FIR_BOOL);
        node->boolean = strcmp(p->tok.text, "true") == 0;
        advance(p);
        return node;
    }
    case TOK_NUMBER: {
        FirNode *node = fir_node_new(FIR_NUMBER);
        node->number = strtod(p->tok.text, NULL);
        advance(p);
        return node;
    }
    case TOK_LBRACE:
        return parse_map(p);
    case TOK_LBRACKET:
        return parse_list(p);
    default:
        fail(p, "expected value");
        return NULL;
    }
}

FirNode *fir_parse(const char *src) {
    Parser p;
    lexer_init(&p.lx, src);
    p.tok.text = NULL;
    advance(&p);

    FirNode *root = parse_map_body(&p, TOK_EOF);
    token_free(&p.tok);
    return root;
}
