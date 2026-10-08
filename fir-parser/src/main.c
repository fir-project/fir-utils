#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"
#include "render.h"

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc((size_t)len + 1);
    size_t got = fread(buf, 1, (size_t)len, f);
    buf[got] = '\0';
    fclose(f);
    return buf;
}

static char *replace_extension(const char *path, const char *ext) {
    const char *slash = strrchr(path, '/');
    const char *dot = strrchr(path, '.');
    if (!dot || (slash && dot < slash))
        dot = path + strlen(path);

    size_t stem = (size_t)(dot - path);
    size_t extlen = strlen(ext);
    char *out = malloc(stem + extlen + 1);
    memcpy(out, path, stem);
    memcpy(out + stem, ext, extlen + 1);
    return out;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <config.fir> [-o output.txt]\n", argv[0]);
        return 1;
    }

    const char *input = argv[1];
    char *output = NULL;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
            output = argv[++i];
    }

    char *src = read_file(input);
    if (!src) {
        fprintf(stderr, "error: cannot read '%s'\n", input);
        return 1;
    }

    FirNode *root = fir_parse(src);
    free(src);
    if (!root) {
        fprintf(stderr, "error: failed to parse '%s'\n", input);
        return 1;
    }

    char *default_out = NULL;
    if (!output) {
        default_out = replace_extension(input, ".txt");
        output = default_out;
    }

    FILE *out = fopen(output, "w");
    if (!out) {
        fprintf(stderr, "error: cannot write '%s'\n", output);
        fir_node_free(root);
        free(default_out);
        return 1;
    }

    if (!fir_render(out, root))
        fprintf(stderr, "warning: no 'system' block found in '%s'\n", input);

    fclose(out);
    fir_node_free(root);
    free(default_out);
    return 0;
}
