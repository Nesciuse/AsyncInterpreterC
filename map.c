#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "map.h"

static int map_get_index(MapObject *map, const char *key);
static void map_bloat(MapObject *map);

Variable def = {.i=0};

#define MAP_DEFAULT_SIZE 10
#define new_map() new_default_map(def)
MapObject *new_default_map(Variable def_val) {
    MapObject *map = malloc(sizeof(MapObject));
    *map = (MapObject){
        .elements = 0,
        .size = MAP_DEFAULT_SIZE,
        .def_val = def_val,
        .keys = malloc(sizeof(char *) * MAP_DEFAULT_SIZE),
        .values = malloc(sizeof(Variable) * MAP_DEFAULT_SIZE),
        .parent = NULL
    };
    return map;
}

Variable map_set_default(MapObject *map, Variable default_value) {
    Variable original = map->def_val;
    map->def_val = default_value;
    return original;
}

void map_free(MapObject *map) {
    free(map->keys);
    for(int i = 0; i < map->elements; i++) {
        if(map->values[i].type == Dictionary) {
            map_free(map->values[i].p);
        }
    }
    free(map->values);
    free(map);
}

static int map_get_index(MapObject *map, const char *key) {
    for(int i = 0; i < map->elements; i++) {
        if(strcmp(key, map->keys[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static void map_bloat(MapObject *map) {
    //todo double size
    fprintf(stderr, "map full, todo map.h line: %d\n", __LINE__);
    exit(1);
}

int map_set(MapObject *map, const char *key, Variable value) {
    int i = map_get_index(map, key);
    if(i == -1) {
        for(MapObject *parent = map->parent; map->parent != NULL; parent = parent->parent) {
            int i = map_get_index(parent, key);
            if(i != -1) {
                parent->values[i] = value;
                return 1;
            }
        }
        int last_element_index = map->elements;
        if(last_element_index == map->size) {
            map_bloat(map);
        }
        map->keys[last_element_index] = key;
        map->values[last_element_index] = value;
        map->elements++;
        return 0;
    }

    map->values[i] = value;
    return 1;
}

void map_setint(MapObject *map, const char *key, int value) {
    map_set(map, key, (Variable){.i = value});
}

Variable map_remove(MapObject *map, const char *key) {
    int ix = map_get_index(map, key);
    if(ix == -1) {
        return def;
    }
    //possibly memory leak if value or key was allocated using malloc
    int last_ix = --(map->elements);
    if(ix == last_ix) {
        return map->values[ix]; 
    }

    Variable original = map->values[ix];
    map->keys[ix] = map->keys[last_ix];
    map->values[ix] = map->values[last_ix];
    return original;
}

Variable map_get(MapObject *map, const char *key) {
    int i = map_get_index(map, key);
    if(i == -1) {
        if(map->parent) {
            return map_get(map->parent, key);
        }
        return map->def_val;
    }
    return map->values[i];
}
