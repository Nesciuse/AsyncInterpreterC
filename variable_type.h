#ifndef H_VARIABLE_TYPE
#define H_VARIABLE_TYPE

typedef enum  {
    Null,
    Integer,
    Float,
    String,
    KeyWord,
    Builtin,
    Var,
    Reference,
    Eval,
    Dictionary,
    End,
    CodeBlock,
    ProgramPointer,
    WaitingObject
} VariableType;

extern const char *VariableTypeString[];

struct variable {
    VariableType type;
    union {
        const char *s;
        int i;
        float f;
        void *p;
        struct variable (*pf)();
    };
};
typedef struct variable Variable;

#define null() {.type=Null,.i=0}

#define integer(num) {.type=Integer,.i=num}
#define str(txt) {.type=String,.s=txt}
#define var(id) {.type=Var,.s=#id}
#define keyword(key) {.type=KeyWord,.i=key}
#define func(program) {.type=ProgramPointer,.p=program}
#define builtin(function) {.type=Builtin,.pf=function}

#endif