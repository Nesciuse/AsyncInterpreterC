#include <stdio.h>
#include <glib.h>

#include "builtins.h"

BUILTIN_FUNC(builtin_print) {
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

BUILTIN_FUNC(builtin_async_sleep) {
    Variable *line = c->program[c->current_line];
    Variable v = get_argument_value(c, 1);
    if(v.type != Integer) {
        fprintf(stderr, " asleep takes integer as argument ");
        exit(1);
    } 
    *c->waiting = 1;
    g_timeout_add_once(v.i, run_context, c);
}

static void impulse() {
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

BUILTIN_FUNC(builtin_async_move_forward_start) {
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
