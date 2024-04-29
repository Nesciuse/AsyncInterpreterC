#include <stdio.h>
#include "interpreter.h"
#include "builtins.h"

Program p5 = {
    {Set(a), integer(20)},
    {sleep, integer(100)},
    {While, eval(a), codeblock(
        {print, str("current value of a: ")},
        {print, var(a)},
        {print, str("\n")},
        {Set(a), eval(a-1)},
        {Set(a2), integer(10)},
        {While, eval(a2), codeblock(
            {sleep, integer(100)},
            {print, str("    current value of a2: ")},
            {print, var(a2)},
            {print, str("\n")},
            {If, eval(a2==5), codeblock(
                {print, str("\na2 is equal to 5\nSleeping for 1 second\n")},
                {sleep, integer(1000)},
            )},
            {Set(a2), eval(a2-1)},
        )},
    )},
    END
};

Program p4 = {
    {moveforward, integer(50)},
    {Set(a), integer(5)},
    {Set(b), integer(50)},
    {Set(a), eval(a + b)},
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
    {Set(a), integer(5)},
    {Set(b), integer(50)},
    {Set(a), eval(a + b)},
    {print, str("\nProgram 3 a + b = ")},
    {print, var(a)},
    {print, str("\n")},
    END
};


Program p2 = {
    {Set(a), integer(775)},
    {Set(b), integer(506)},
    {Set(a), eval(2*(a + b))},
    {Set(eval_test), eval(a + b)},
    {print, var(eval_test)},
    {print, str("\n")},
    {sleep, integer(3000)},
    {print, str("\nSlept for 3000 miliseconds\n")},
    END
};

Program factorial_5 = {
    {Set(i), integer(5)},
    {Set(f), integer(1)},
    {sleep, integer(1000)},
    {While, eval(i>1), codeblock(
        {Set(f), eval(f*i)},
        {Set(i), eval(i-1)},
        {sleep, integer(10)},
        {print, var(f)},
        {print, str("\n")},
    )},
    {Return, eval(f)}
};

Program big = {
    {Set(i), integer(8)},
    {Set(f), integer(1)},
    {sleep, integer(1000)},
    {While, eval(i>1), codeblock(
        {Set(f), eval(f*i)},
        {Set(i), eval(i-1)},
        {sleep, integer(10)},
        {print, var(f)},
        {print, str("\n")},
        {If, eval(f>200000), codeblock(
            {Return, var(f)},
        )},
    )},
    {Return, var(f)}
};

Program print_retnum = {
    {Set(b), func(factorial_5)},
    {print, str("!5 = ")},
    {print, var(b)},
    {Set(b), func(big)},
    {print, str("\nbig = ")},
    {print, var(b)},
    END
};

#define $(...) {__VA_ARGS__ , LINE_END},
#define intvar(a, b) $(Set(a), integer(b))
#define setvar(a, b) $(Set(a), eval(b))
Program p1 = {
    intvar(a, 775)
    intvar(b, 606)
    setvar(a, a+a)
    setvar(b, a+b)
    $( print, var(a) )
    $( print, str("\n") )
    $( sleep, var(b) )
    $( print, str("\nSlept for ") )
    $( print, var(b) )
    $( print, str(" miliseconds\n") )
    END
};

#define TESTING 0

void print_end(void *text) {
    printf("%s", (const char *) text);
}

int main(int argc, char **argv) {
    init_interpreter();

    if(TESTING) {
        printf("Starting testing\n");
        eval_testing();

        return 0;
    }

    start_program(print_retnum, print_end, "Calling program ended\n");
    // start_program(p1, print_end, "Program 1 ended\n");
    // start_program(p2, print_end, "Program 2 ended\n");
    // start_program(p4, print_end, "Program 4 ended\n");
    // start_program(p5, print_end, "Program 5 ended\n");
    
    start_interpreter();
    return 0;
}

//gcc.exe (GCC) 12.2.0
//gcc -o test interpreter.c map.c -LC:/lib -"lglib-2.0" -lintl -IC:/include/glib-2.0 -IC:/lib/glib-2.0/include