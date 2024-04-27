#ifndef H_MAP
#define H_MAP

#include "variable_type.h"

struct map_struct {
    int size;
    int elements;
    const char **keys;
    Variable *values;
    Variable def_val;
    struct map_struct *parent;
};
typedef struct map_struct MapObject;

MapObject *new_default_map(Variable def_val);
void map_free(MapObject *map);
Variable *map_set(MapObject *map, const char *key, Variable value);
void map_setint(MapObject *map, const char *key, int value);
Variable map_get(MapObject *map, const char *key);
Variable map_remove(MapObject *map, const char *key);

Variable evaluate(MapObject *locals, const char *expr);

#define new_map() new_default_map(def)

extern Variable def;

void eval_testing();

#endif