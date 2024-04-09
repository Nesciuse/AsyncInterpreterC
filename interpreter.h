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
    Sleep = 0, 
    Print,
    Return,
    Call,
    Less,
    WhileKey,
    JumpIf,
    Async,
    IfKey,
    MoveForward,
    MoveHeadUp,
    MoveLeft,
    program_end,
    codeblock_end,
    line_end
};

void _print(Context *c);

#define add keyword(Add)
#define print keyword(Print)
#define sleep keyword(Sleep)
#define async keyword(Async)
#define moveforward keyword(MoveForward)
#define If keyword(IfKey)
#define While keyword(WhileKey)

#define eval(expr) {.type=Eval,.s=#expr}

#define END {{.type=End,.i=program_end}}
#define CODEBLOCK_END {{.type=End,.i=codeblock_end}}
#define LINE_END {.type=End,.i=line_end}

#define codeblock(...) {.type=CodeBlock, .p=(Program){__VA_ARGS__ CODEBLOCK_END}}

#define program(name, ...) Variable name = codeblock(__VA_ARGS__)


Context *new_context();
Context *new_subcontext();
void free_context(Context *c);
void free_subcontext(Context *c);
void run_context(void *);
void asleep(Context *c);
void _add(Context *c);
gboolean a_move_forward(void *p);
void a_move_forward_start(Context *c);
void start_program(Program program, CallbackFunctionPointer final_callback, void *data);
void print_end(void *text);
void quit(void *loop);

extern GMainLoop *loop;

#endif