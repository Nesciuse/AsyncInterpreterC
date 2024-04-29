#include <stdio.h>
#include <glib.h>
#include <math.h>
#include <time.h>

#include "interpreter.h"
#include "builtins.h"

GMainLoop *loop = NULL;

static void quit_interpreter() {
    printf("Quitting main loop\n");

    g_main_loop_quit(loop);
    g_main_loop_unref(loop);

    printf("\n\nProgram finished\n\n");
}

void init_interpreter() {
    printf("Interpreter starting\n\n");
    loop = g_main_loop_new(NULL, FALSE);
}

void start_interpreter() {
    printf("\nStarting main loop\n");    

    g_main_loop_run(loop);
}

Variable null = {.type=Null,.i=0};

extern Variable evaluate(MapObject * ,const char *);
Variable get_argument_value(Context *c, int argix) {
    Variable v = c->program[c->current_line][argix];
    switch(v.type) {
        case Eval:
            return evaluate(c->locals, v.s);
        case Var:
            return map_get(c->locals, v.s);
        // case KeyWord:
        //     if(v.i == Call) {
        //         return (Variable){.type=WaitingObject, .i=0};
        //     }
    }
    return v;
}

Context *new_context() {
    Context *c = malloc(sizeof(Context));
    c->status = malloc(sizeof(int));
    *c->status = Running;
    c->locals = new_map();
    c->current_line = 0;
    c->subcontext = c->supercontext = NULL;
    return c;
}

Context *new_subcontext(Context *supercontext, Program p) {
    Context *c = malloc(sizeof(Context));
    c->status = supercontext->status;
    c->supercontext = supercontext;
    c->program = p;
    c->current_line = 0;
    supercontext->subcontext = c;
    c->locals = new_map();
    c->locals->parent = supercontext->locals;
    c->subcontext = NULL;
    return c;
}

static inline void run_codeblock(Context *c, void *codeblock) {
    *c->status = Waiting;
    g_timeout_add_once(0, run_context, new_subcontext(c, codeblock));
}

void free_context(Context *c) {
    if(c->supercontext) {
        c->supercontext->subcontext = NULL;
    }
    else {
        free(c->status);
    }
    if(c->subcontext) {
        free_context(c->subcontext);
    }
    map_free(c->locals);
    free(c);
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
        run_codeblock(c, args[2].p);
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
        run_codeblock(c, args[2].p);
    }
}

struct ret {
    Context *caller;
    Context *callee;
};

void set_return(void *d) {
    struct ret *r = d;
    Variable *retadr = r->caller->custom_data;
    r->caller->custom_data = NULL;

    Variable retval = *((Variable *) r->callee->custom_data);
    r->callee->custom_data = NULL;

    *retadr = retval;

    Context *c = r->caller;
    free(r);
    g_timeout_add_once(0, run_context, c);
}

int running_programs = 0;

void program_call(Context *c, void *program, Variable *return_address) {
    *c->status = Waiting;
    c->custom_data = return_address;

    Context *newctx = new_context();
    newctx->program = program;
    newctx->final_callback = set_return;
    newctx->data = malloc(sizeof(struct ret));
    *((struct ret *)newctx->data) = (struct ret){.caller=c, .callee=newctx};
    
    running_programs++;
    g_timeout_add_once(0, run_context, newctx);
}

void set_key(Context *c) {
    Variable *args = c->program[c->current_line];
    Variable var_name = args[1];
    Variable value = get_argument_value(c, 2);
    
    if(value.type == ProgramPointer) {
        Variable *return_address = map_set(c->locals, var_name.s, null);
        program_call(c, value.p, return_address);
    }
    else {
        map_set(c->locals, var_name.s, value);
    }
}

void (*KeywordMap[])(Context *) = {
    [IfKey] = if_statement,
    [WhileKey] = while_statement,
    [SetKey] = set_key
};



static inline Context *collapse_subcontexts(Context *c) {
    Context *cur = c, *sup;
    while((sup = cur->supercontext) != NULL) {
        free_context(cur);
        cur = sup;
    }
    return cur;
}

static inline void finish_context(Context *c) {
    c->final_callback(c->data);
    free_context(c);
    if(--running_programs == 0) {
        quit_interpreter();
    }
}

void run_context(void *d) {
    Context *c = (Context *)d;
    
    switch(*c->status) {
        case Waiting:
            *c->status = Running;
            break;
        case Ended:
            finish_context(collapse_subcontexts(c));
            return;
    }
    
    while(1) {
        Variable *line = c->program[c->current_line];
        Variable arg0 = line[0];
        switch(arg0.type) {
            case End:
                switch(arg0.i) {
                    case program_end:
                        Variable retval = get_argument_value(c, 1);
                        Context *root = collapse_subcontexts(c);
                        root->custom_data = &retval;
                        finish_context(root);
                        return;

                    case codeblock_end:
                        if(c->supercontext == NULL) {
                            fprintf(stderr, " codeblock end without supercontext ???");
                            exit(1);
                        }
                        Context *supercontext = c->supercontext;
                        free_context(c);
                        g_timeout_add_once(0, run_context, supercontext);
                        return;

                    default:
                        fprintf(stderr, " invalid End symbol");
                        exit(1);
                }

            case Builtin:
                arg0.pf(c);
                break;

            case KeyWord:
                KeywordMap[arg0.i](c);
                break;
            }
            
        c->current_line++;
        if(*c->status) {
            return;
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


