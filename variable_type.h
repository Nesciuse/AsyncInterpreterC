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