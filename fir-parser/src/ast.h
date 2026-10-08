#ifndef FIR_AST_H
#define FIR_AST_H

#include <stddef.h>

typedef enum {
    FIR_STRING,
    FIR_BOOL,
    FIR_NUMBER,
    FIR_MAP,
    FIR_LIST
} FirNodeKind;

typedef struct FirNode FirNode;

typedef struct {
    char *key;
    FirNode *value;
} FirEntry;

struct FirNode {
    FirNodeKind kind;
    union {
        char *string;
        int boolean;
        double number;
        struct {
            FirEntry *items;
            size_t count;
            size_t cap;
        } map;
        struct {
            FirNode **items;
            size_t count;
            size_t cap;
        } list;
    };
};

FirNode *fir_node_new(FirNodeKind kind);
void fir_node_free(FirNode *node);
void fir_map_add(FirNode *map, char *key, FirNode *value);
void fir_list_add(FirNode *list, FirNode *value);
FirNode *fir_map_get(const FirNode *map, const char *key);

#endif
