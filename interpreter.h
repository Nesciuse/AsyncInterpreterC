#ifndef H_INTERPRETER
#define H_INTERPRETER

#include "map.h"

typedef Variable ProgramLine[4];
typedef ProgramLine Program[];

typedef void (*CallbackFunctionPointer)(void*);
struct context_obj {
    int current_line;
    ProgramLine *program;
    MapObject *locals;
    void *custom_data;
    CallbackFunctionPointer final_callback;
    int *waiting;
    struct context_obj *supercontext;
    struct context_obj *subcontext;
    void *data;
};
typedef struct context_obj Context;

enum {
    Call,
    WhileKey,
    IfKey,
    SetKey,
    program_end,
    codeblock_end,
    line_end
};

#define If keyword(IfKey)
#define While keyword(WhileKey)

#define call keyword(Call)

#define Set(v) keyword(SetKey), {.type=Var,.s=#v}

#define eval(expr) {.type=Eval,.s=#expr}

#define null {.type=Null,.i=0}
#define Return {.type=End,.i=program_end}
#define END {Return, null}

#define CODEBLOCK_END {{.type=End,.i=codeblock_end}}
#define LINE_END {.type=End,.i=line_end}

#define codeblock(...) {.type=CodeBlock, .p=(Program){__VA_ARGS__ CODEBLOCK_END}}

#define program(name, ...) Variable name = codeblock(__VA_ARGS__)


Context *new_context();
Context *new_subcontext();
void free_context(Context *c);
void run_context(void *);
void builtin_async_sleep(Context *c);
gboolean a_move_forward(void *p);
void a_move_forward_start(Context *c);
void start_program(Program program, CallbackFunctionPointer final_callback, void *data);
void print_end(void *text);
void quit(void *loop);

Variable get_argument_value(Context *c, int argix);

extern GMainLoop *loop;

#endif