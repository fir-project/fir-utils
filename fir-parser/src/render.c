#include <ctype.h>
#include <string.h>

#include "render.h"

#define INDENT_SMALL "     "
#define INDENT_LIST "        "

static const char *last_segment(const char *key) {
    const char *dot = strrchr(key, '.');
    return dot ? dot + 1 : key;
}

static void fputs_upper(FILE *out, const char *s) {
    for (; *s; s++)
        fputc(toupper((unsigned char)*s), out);
}

static void render_users(FILE *out, const FirNode *system) {
    for (size_t i = 0; i < system->map.count; i++) {
        const char *key = system->map.items[i].key;
        const FirNode *block = system->map.items[i].value;
        if (strncmp(key, "user.", 5) != 0 || block->kind != FIR_MAP)
            continue;

        const char *name = last_segment(key);
        fputs("CREATE USER ", out);
        fputs_upper(out, name);
        fputc('\n', out);

        const FirNode *shell = fir_map_get(block, "shell");
        if (shell && shell->kind == FIR_STRING) {
            fputs("SET SHELL ", out);
            fputs(shell->string, out);
            fputc('\n', out);
        }

        const FirNode *perms = fir_map_get(block, "permissions");
        if (!perms || perms->kind != FIR_LIST)
            continue;

        fputs("GIVE USER ", out);
        fputs_upper(out, name);
        fputs(" PERMISSIONS:\n", out);
        for (size_t j = 0; j < perms->list.count; j++) {
            const FirNode *perm = perms->list.items[j];
            if (perm->kind != FIR_STRING)
                continue;
            fputs(INDENT_SMALL, out);
            fputs_upper(out, perm->string);
            fputc('\n', out);
        }
    }
}

static void render_package_group(FILE *out, const char *label, const FirNode *list) {
    fputs(INDENT_SMALL, out);
    fputs_upper(out, label);
    fputs(":\n", out);
    for (size_t i = 0; i < list->list.count; i++) {
        const FirNode *pkg = list->list.items[i];
        if (pkg->kind != FIR_STRING)
            continue;
        fputs(INDENT_LIST, out);
        fputs_upper(out, pkg->string);
        fputc('\n', out);
    }
}

static void render_packages(FILE *out, const FirNode *system) {
    const FirNode *packages = fir_map_get(system, "packages");
    if (!packages || packages->kind != FIR_MAP)
        return;

    fputs("\nPACKAGES:\n", out);
    for (size_t i = 0; i < packages->map.count; i++) {
        const char *key = packages->map.items[i].key;
        const FirNode *list = packages->map.items[i].value;
        if (list->kind != FIR_LIST)
            continue;

        if (strcmp(key, "global") == 0) {
            render_package_group(out, "GLOBAL", list);
        } else if (strncmp(key, "user.", 5) == 0) {
            const char *label = last_segment(key);
            char buf[128];
            snprintf(buf, sizeof(buf), "USER %s", label);
            render_package_group(out, buf, list);
        }
    }
}

int fir_render(FILE *out, const FirNode *root) {
    const FirNode *system = fir_map_get(root, "system");
    if (!system || system->kind != FIR_MAP)
        return 0;

    fputs("FIR CONFIG BUILD:\n\n", out);

    const FirNode *hostname = fir_map_get(system, "hostname");
    if (hostname && hostname->kind == FIR_STRING) {
        fputs("HOSTNAME ", out);
        fputs_upper(out, hostname->string);
        fputc('\n', out);
    }

    render_users(out, system);
    render_packages(out, system);
    return 1;
}
