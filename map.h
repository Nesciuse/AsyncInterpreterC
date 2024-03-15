#include "variable_type.h"

struct map_struct {
    int size;
    int elements;
    const char **keys;
    Variable *values;
    Variable def_val;
    void (*set)(struct map_struct *, const char *, Variable);
    void (*setint)(struct map_struct *, const char *, int);
    Variable (*get)(struct map_struct *, const char *);
    Variable (*remove)(struct map_struct *, const char *);
};
typedef struct map_struct MapObject;

MapObject *new_default_map(Variable def_val);
void map_free(MapObject *map);

Variable evaluate(MapObject *locals, const char *expr);

#define new_map() new_default_map(def)

extern Variable def;

void eval_testing();