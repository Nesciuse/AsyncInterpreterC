#include <stdio.h>
#include <glib.h>
#include <math.h>
#include <time.h>

#include "interpreter.h"

Variable get_argument_value(Context *c, int argix) {
    Variable v = c->program[c->current_line][argix];
    switch(v.type) {
        case Eval:
            return evaluate(c->locals, v.s);
        case Var:
            return map_get(c->locals, v.s);
        case KeyWord:
            if(v.i == Call) {
                return (Variable){.type=WaitingObject, .i=0};
            }
    }
    return v;
}

void _print(Context *c) {
    Variable v = get_argument_value(c, 1);
    switch(v.type) {
        case Float:
            printf("%f", v.f);
            break;
        case Integer:
            printf("%d", v.i);
            break;
        case String:
            printf("%s", v.s);
    }
}

GMainLoop *loop;
Context *new_context() {
    Context *c = malloc(sizeof(Context));
    c->waiting = malloc(sizeof(int));
    *c->waiting = 0;
    c->locals = new_map();
    c->current_line = 0;
    c->subcontext = c->supercontext = NULL;
    return c;
}

Context *new_subcontext(Context *supercontext, Program p) {
    Context *c = malloc(sizeof(Context));
    c->waiting = supercontext->waiting;
    c->supercontext = supercontext;
    c->program = p;
    c->current_line = 0;
    supercontext->subcontext = c;
    c->locals = new_map();
    c->locals->parent = supercontext->locals;
    c->subcontext = NULL;
    return c;
}

void free_context(Context *c) {
    if(c->supercontext) {
        c->supercontext->subcontext = NULL;
    }
    else {
        free(c->waiting);
    }
    if(c->subcontext) {
        free_context(c->subcontext);
    }
    map_free(c->locals);
    free(c);
}

void run_context(void *d);
void asleep(Context *c) {
    Variable *line = c->program[c->current_line];
    Variable v = get_argument_value(c, 1);
    if(v.type != Integer) {
        fprintf(stderr, " asleep takes integer as argument ");
        exit(1);
    } 
    *c->waiting = 1;
    g_timeout_add_once(v.i, run_context, c);
}

void impulse() {
    printf(" #> ");
}

gboolean a_move_forward(void *p) {
    Context *c = (Context *)p;
    if((*(int *)c->custom_data)-- > 0) {
        impulse();
        *c->waiting = 1;
        return TRUE;
        //g_timeout_add_once(100, a_move_forward, c);
    }
    else {
        free(c->custom_data);
        *c->waiting = 1;
        g_timeout_add_once(0, run_context, c);
    }
    return FALSE;
}

void a_move_forward_start(Context *c) {
    Variable *args = c->program[c->current_line];
    Variable v = get_argument_value(c, 1);
    if(v.type != Integer) {
        fprintf(stderr, " moveforward takes integer as argument ");
        exit(1);
    } 
    int steps = v.i;
    c->custom_data = malloc(sizeof(int));
    *((int *)c->custom_data) = steps;
    *c->waiting = 1;
    g_timeout_add(100, a_move_forward, c);
}

void if_statement(Context *c) {
    Variable *args = c->program[c->current_line];
    Variable condition = get_argument_value(c, 1);
    int is_true = 1;
    switch(condition.type) {
        case Integer:
            is_true = condition.i != 0;
            break;
        case Float:
            is_true = condition.f != 0.0;
            break;
        case Null:
            is_true = 0;
            break;
        default:
            fprintf(stderr, " unimplemented type in if check ");
            exit(1);
    }
    if(is_true) {
        *c->waiting = 1;
        g_timeout_add_once(0, run_context, new_subcontext(c, args[2].p));
    }
}

void while_statement(Context *c) {
    Variable *args = c->program[c->current_line];
    Variable condition = get_argument_value(c, 1);
    int is_true = 1;
    switch(condition.type) {
        case Integer:
            is_true = condition.i != 0;
            break;
        case Float:
            is_true = condition.f != 0.0;
            break;
        case Null:
            is_true = 0;
            break;
        default:
            fprintf(stderr, " unimplemented type in while check ");
            exit(1);
    }

    if(is_true) {
        c->current_line--;
        *c->waiting = 1;
        g_timeout_add_once(0, run_context, new_subcontext(c, args[2].p));
    }
}

void program_call(Context *c) {
    Variable p = get_argument_value(c, 1);
    if(p.type != ProgramPointer) {
        fprintf(stderr, " can call only another program ");
        exit(1);
    }
}

void set_key(Context *c) {
    Variable *args = c->program[c->current_line];
    Variable var_name = args[1];
    Variable value = get_argument_value(c, 2);
    
    if(value.type == WaitingObject) {
        *c->waiting = 1;
        Variable prog = get_argument_value(c, 3);
        Context *sub = new_subcontext(c, prog.p);
        g_timeout_add_once(0, run_context, sub);
        Variable *return_address = map_set(c->locals, var_name.s, value);
        c->custom_data = return_address->p = return_address;
    }
    else {
        map_set(c->locals, var_name.s, value);
    }
}

void (*FunctionMap[])(Context *) = {
    [Print] = _print,
    [Sleep] = asleep,
    [MoveForward] = a_move_forward_start,
    [IfKey] = if_statement,
    [WhileKey] = while_statement,
    [Call] = program_call,
    [SetKey] = set_key
};

int running_programs = 0;
void run_context(void *d) {
    Context *c = (Context *)d;
    *c->waiting = 0;
    while(1) {
        Variable *line = c->program[c->current_line];
        switch(line[0].type) {
            case End:
                if(c->supercontext) {
                    Context *supercontext = c->supercontext;
                    if(line[0].i == program_end && supercontext->custom_data != NULL) {
                        Variable *return_address = supercontext->custom_data;
                        supercontext->custom_data = NULL;
                        *return_address = get_argument_value(c, 1);
                    }
                    free_context(c);
                    g_timeout_add_once(0, run_context, supercontext);
                    return;
                }
                c->final_callback(c->data);
                free_context(c);
                if(--running_programs == 0) {
                    g_main_loop_quit(loop);
                }
                return;

            case KeyWord:
                FunctionMap[line[0].i](c);
                c->current_line++;
                if(*c->waiting) {
                    return;
                }
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
