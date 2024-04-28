#ifndef H_BUILTINS
#define H_BUILTINS

#include "interpreter.h"

#define BUILTIN_FUNC(name) void name(Context *c)

BUILTIN_FUNC(builtin_print);
#define print builtin(builtin_print)

BUILTIN_FUNC(builtin_async_sleep);
#define sleep builtin(builtin_async_sleep)

BUILTIN_FUNC(builtin_async_move_forward_start);
#define moveforward builtin(builtin_async_move_forward_start)

#endif