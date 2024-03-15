#include "map.h"

typedef Variable ProgramLine[4];
typedef ProgramLine Program[];

typedef void (*CallbackFunctionPointer)(void*);
typedef struct {
    int current_line;
    ProgramLine *program;
    MapObject *locals;
    void *custom_data;
    CallbackFunctionPointer final_callback;
    void *data;
} Context;

enum {
    Sleep = 0, 
    Print,
    Add,
    Return,
    Call,
    Less,
    While,
    JumpIf,
    Async,
    MoveForward,
    MoveHeadUp,
    MoveLeft,
    func_enum_size,
    program_end
};

typedef void (*Arg1Func)(Context *, Variable);
typedef void (*Arg2Func)(Context *, Variable, Variable);
typedef void (*Arg3Func)(Context *, Variable, Variable, Variable);

void _print(Context *c);

#define add keyword(Add)
#define print keyword(Print)
#define sleep keyword(Sleep)
#define async keyword(Async)
#define moveforward keyword(MoveForward)
#define eval(expr) {.type=Eval,.s=#expr}
#define END {{.type=End,.i=program_end}}

Context *new_context();
void free_context(Context *c);
void run_context(void *);
void asleep(Context *c);
void _add(Context *c);
gboolean a_move_forward(void *p);
void a_move_forward_start(Context *c);
void start_program(Program program, CallbackFunctionPointer final_callback, void *data);
void print_end(void *text);
void quit(void *loop);

extern GMainLoop *loop;