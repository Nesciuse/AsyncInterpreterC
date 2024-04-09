#include <stdio.h>
#include <glib.h>
#include "interpreter.h"



Program p5 = {
    {var(a), integer(20)},
    {sleep, integer(100)},
    {While, eval(a), codeblock(
        {print, str("current value of a: ")},
        {print, var(a)},
        {print, str("\n")},
        {var(a), eval(a-1)},
        {var(a2), integer(10)},
        {While, eval(a2), codeblock(
            {sleep, integer(100)},
            {print, str("    current value of a2: ")},
            {print, var(a2)},
            {print, str("\n")},
            {moveforward, var(a2)},
            {var(a2), eval(a2-1)},
        )},
    )},
    END
};

Program p4 = {
    {moveforward, integer(50)},
    {var(a), integer(5)},
    {var(b), integer(50)},
    {var(a), eval(a + b)},
    {If, eval(a<b), codeblock(
        {print, var(a)},
        {print, str(" is smaller than ")},
        {print, var(b)},
    )},
    {If, eval(a>b), codeblock(
        {print, var(a)},
        {print, str(" is bigger than ")},
        {print, var(b)},
    )},
    {If, eval(a==b), codeblock(
        {print, var(a)},
        {print, str(" is equal to ")},
        {print, var(b)},
    )},
    {print, var(a)},
    {print, str("\n")},
    END
};

Program p3 = {
    {moveforward, integer(50)},
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
    {sleep, integer(3000)},
    {print, str("\nSlept for 3000 miliseconds\n")},
    END
};

Program p1 = {
    {var(a), integer(775)},
    {var(b), integer(606)},
    {add, var(a), var(a)},
    {add, var(b), var(a)},
    {print, var(a)},
    {print, str("\n")},
    {sleep, var(b)},
    {print, str("\nSlept for ")},
    {print, var(b)},
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
    start_program(p4, print_end, "Program 4 ended\n");
    start_program(p5, print_end, "Program 5 ended\n");

    printf("Starting main loop\n");    
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    printf("Program finished\n");
    return 0;
}

//gcc.exe (GCC) 12.2.0
//gcc -o test interpreter.c map.c -LC:/lib -"lglib-2.0" -lintl -IC:/include/glib-2.0 -IC:/lib/glib-2.0/include