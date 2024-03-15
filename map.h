typedef enum  {
    Integer = 0,
    Float,
    String,
    KeyWord,
    Var,
    Reference,
    Eval,
    Dictionary,
    End,
    Null
} VariableType;

extern const char *VariableTypeString[];

typedef struct {
    VariableType type;
    union {
        const char *s;
        int i;
        float f;
        void *p;
    };
} Variable;

#define integer(num) {.type=Integer,.i=num}
#define str(txt) {.type=String,.s=txt}
#define var(id) {.type=Var,.s=#id}
#define keyword(key) {.type=KeyWord,.i=key}

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