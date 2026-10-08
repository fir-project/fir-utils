#include <stdlib.h>
#include <string.h>

#include "ast.h"

FirNode *fir_node_new(FirNodeKind kind) {
    FirNode *node = calloc(1, sizeof(FirNode));
    node->kind = kind;
    return node;
}

void fir_node_free(FirNode *node) {
    if (!node)
        return;
    switch (node->kind) {
    case FIR_STRING:
        free(node->string);
        break;
    case FIR_MAP:
        for (size_t i = 0; i < node->map.count; i++) {
            free(node->map.items[i].key);
            fir_node_free(node->map.items[i].value);
        }
        free(node->map.items);
        break;
    case FIR_LIST:
        for (size_t i = 0; i < node->list.count; i++)
            fir_node_free(node->list.items[i]);
        free(node->list.items);
        break;
    default:
        break;
    }
    free(node);
}

void fir_map_add(FirNode *map, char *key, FirNode *value) {
    if (map->map.count == map->map.cap) {
        map->map.cap = map->map.cap ? map->map.cap * 2 : 8;
        map->map.items = realloc(map->map.items, map->map.cap * sizeof(FirEntry));
    }
    map->map.items[map->map.count].key = key;
    map->map.items[map->map.count].value = value;
    map->map.count++;
}

void fir_list_add(FirNode *list, FirNode *value) {
    if (list->list.count == list->list.cap) {
        list->list.cap = list->list.cap ? list->list.cap * 2 : 8;
        list->list.items = realloc(list->list.items, list->list.cap * sizeof(FirNode *));
    }
    list->list.items[list->list.count++] = value;
}

FirNode *fir_map_get(const FirNode *map, const char *key) {
    if (!map || map->kind != FIR_MAP)
        return NULL;
    for (size_t i = 0; i < map->map.count; i++) {
        if (strcmp(map->map.items[i].key, key) == 0)
            return map->map.items[i].value;
    }
    return NULL;
}
