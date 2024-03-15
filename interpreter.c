#include <stdio.h>
#include <glib.h>
#include <math.h>
#include <time.h>
#include "interpreter.h"

void _print_float(Variable v) { printf("%f", v.f); }
void _print_integer(Variable v) { printf("%d", v.i); }
void _print_string(Variable v) { printf("%s", v.s); }
void (*PrintMap[])(Variable) = {
    [Float] = _print_float,
    [Integer] = _print_integer,
    [String] = _print_string
};
void _print(Context *c) {
    Variable v = c->program[c->current_line][1];
    if(v.type == Var) {
        v = c->locals->get(c->locals, v.s);
    }
    PrintMap[v.type](v);
}
GMainLoop *loop;
Context *new_context() {
    Context *c = malloc(sizeof(Context));
    c->locals = new_map();
    c->current_line = 0;
}

void free_context(Context *c) {
    map_free(c->locals);
    free(c);
}

void run_context(void *d);
void asleep(Context *c) {
    Variable *line = c->program[c->current_line];
    Variable v = line[2];
    if(v.type == Var) {
        v = c->locals->get(c->locals, v.s);
    }
    g_timeout_add_once(v.i, run_context, c);
}

static inline int get_int(Context *c, Variable v) {
    if(v.type == Var) {
        return c->locals->get(c->locals, v.s).i;
    }
    return v.i;
}

void _add(Context *c) {
    Variable *line = c->program[c->current_line];
    Variable a = line[1], b = line[2];
    int b_val;
    if(b.type == Var) {
        b_val = c->locals->get(c->locals, b.s).i;
    }
    else {
        b_val = b.i;
    }
    int a_val = c->locals->get(c->locals, a.s).i;
    c->locals->set(c->locals, a.s, (Variable)integer(a_val+b_val));
}

void impulse() {
    printf(" #> ");
};

gboolean a_move_forward(void *p) {
    Context *c = (Context *)p;
    if((*(int *)c->custom_data)-- > 0) {
        impulse();
        return TRUE;
        //g_timeout_add_once(100, a_move_forward, c);
    }
    else {
        free(c->custom_data);
        g_timeout_add_once(0, run_context, c);
    }
    return FALSE;
}

void a_move_forward_start(Context *c) {
    Variable *args = c->program[c->current_line];
    int steps = get_int(c, args[2]);
    c->custom_data = malloc(sizeof(int));
    *((int *)c->custom_data) = steps;
    g_timeout_add(100, a_move_forward, c);
}

void (*FunctionMap[])(Context *) = {
    [Print] = _print,
    [Add] = _add,
    [Sleep] = asleep,
    [MoveForward] = a_move_forward_start
};

int running_programs = 0;
void run_context(void *d) {
    Context *c = (Context *)d;
    while(1) {
        Variable *line = c->program[c->current_line];
        switch(line[0].type) {
            case End:
                c->final_callback(c->data);
                free_context(c);
                if(--running_programs == 0) {
                    g_main_loop_quit(loop);
                }
                return;

            case Var:
                Variable arg1 = line[1];
                if(arg1.type == Eval) {
                    arg1 = evaluate(c->locals, arg1.s);
                }
                c->locals->set(c->locals, line[0].s, arg1);
                c->current_line++;
                continue;

            case KeyWord:
                if(line[0].i == Async) {
                    FunctionMap[line[1].i](c);
                    c->current_line++;
                    return;
                }
                FunctionMap[line[0].i](c);
                c->current_line++;
        }
    }
}

void start_program(Program program, CallbackFunctionPointer final_callback, void *data) {
    Context *c = new_context();
    c->program = program;
    c->final_callback = final_callback;
    c->data = data;
    
    running_programs++;
    run_context(c);
}

void print_end(void *text) {
    printf("%s", (const char *) text);
}

void quit(void *loop) {
    printf("Quitting main loop\n");
    g_main_loop_quit((GMainLoop *)loop);
}