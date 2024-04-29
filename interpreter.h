#ifndef H_INTERPRETER
#define H_INTERPRETER

#include "map.h"

typedef Variable ProgramLine[4];
typedef ProgramLine Program[];

typedef void (*CallbackFunctionPointer)(void*);

enum {
    Running = 0,
    Waiting,
    Ended
};

struct context_obj {
    int current_line;
    int current_arg;
    ProgramLine *program;
    MapObject *locals;
    void *custom_data;
    CallbackFunctionPointer final_callback;
    int *status;
    struct context_obj *supercontext;
    struct context_obj *subcontext;
    void *data;
};
typedef struct context_obj Context;

enum {
    WhileKey,
    IfKey,
    SetKey,
    program_end,
    codeblock_end,
    line_end
};

#define If keyword(IfKey)
#define While keyword(WhileKey)

#define Set(v) keyword(SetKey), {.type=Var,.s=#v}

#define eval(expr) {.type=Eval,.s=#expr}

#define Return {.type=End,.i=program_end}
#define END {Return, null()}

#define CODEBLOCK_END {{.type=End,.i=codeblock_end}}
#define LINE_END {.type=End,.i=line_end}

#define codeblock(...) {.type=CodeBlock, .p=(Program){__VA_ARGS__ CODEBLOCK_END}}

#define program(name, ...) Variable name = codeblock(__VA_ARGS__)

void init_interpreter();
void start_interpreter();

Variable get_argument_value(Context *c, int argix);

Context *new_context();

void free_context(Context *c);
void run_context(void *);
void start_program(Program program, CallbackFunctionPointer final_callback, void *data);

extern Variable null;
#endif