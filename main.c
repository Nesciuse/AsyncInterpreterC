#include <stdio.h>
#include <glib.h>
#include "interpreter.h"

Program p3 = {
    {async, moveforward, integer(50)},
    {var(a), integer(5)},
    {var(b), integer(50)},
    {var(a), eval(a + b)},
    {print, str("\nProgram 3 a + b = ")},
    {print, var(a)},
    {print, str("\n")},
    END
};

Program p2 = {
    {var(a), integer(775)},
    {var(b), integer(506)},
    {var(a), eval(2*(a + b))},
    {var(eval_test), eval(a + b)},
    {print, var(eval_test)},
    {print, str("\n")},
    {async, sleep, integer(3000)},
    {print, str("\nSlept for 3000 miliseconds\n")},
    END
};

Program p1 = {
    {var(test), integer(775)},
    {var(test2), integer(606)},
    {add, var(test2), var(test)},
    {add, var(test2), var(test2)},
    {print, var(test2)},
    {print, str("\n")},
    {async, sleep, var(test2)},
    {print, str("\nSlept for ")},
    {print, var(test2)},
    {print, str(" miliseconds\n")},
    END
};

#define TESTING 0
int main(int argc, char **argv) {
    if(TESTING) {
        printf("Starting testing\n");
        eval_testing();

        return 0;
    }

    printf("Program starting\n");

    loop = g_main_loop_new(NULL, FALSE);

    start_program(p1, print_end, "Program 1 ended\n");
    start_program(p2, print_end, "Program 2 ended\n");
    start_program(p3, print_end, "Program 3 ended\n");

    printf("Starting main loop\n");    
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    printf("Program finished\n");
    return 0;
}

//gcc.exe (GCC) 12.2.0
//gcc -o test interpreter.c map.c -LC:/lib -"lglib-2.0" -lintl -IC:/include/glib-2.0 -IC:/lib/glib-2.0/include