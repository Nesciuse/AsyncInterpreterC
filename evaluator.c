#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "evaluator.h"

static inline void print_error(const char *error, ...) {
    va_list vargv;
    va_start(vargv, error); 
    vfprintf(stderr, error, vargv);
    exit(1);
}

static inline void debug_print(const char *str, ...) {
    #if EVALUATOR_DEBUG == 1
    va_list vargv;
    va_start(vargv, str); 
    vprintf(str, vargv);
    #endif
}

static inline void print_expression(const char *start, const char *end) {
    #if EVALUATOR_PRINT_EXPRESSION == 1
    printf("'");
    for(const char *p = start; p < end; p++) {
        printf("%c", *p);
    }
    printf("'\n");
    #endif
}

static inline void eval_error(Variable a, Variable b, const char *operation) {
    print_error("eval error can't use '%s' between type %s and %s", operation, VariableTypeString[a.type], VariableTypeString[b.type]);
}

Variable operation_or (Variable a, Variable b) {
    switch(a.type) {
        case Integer: if(a.i) return a; break;
        case Float: if(a.f) return a;
    }
    return b;
}

Variable operation_and (Variable a, Variable b) {
    switch(a.type) {
        case Integer: if(a.i) return b; break;
        case Float: if(a.f) return b;
    }
    return a;
}

UNARY_OPERATION_TEMPLATE(unary_minus,-)
UNARY_OPERATION_TEMPLATE(unary_plus,+)
UNARY_OPERATION_TEMPLATE(negation,!)

ARITHEMTIC_OPERATION_TEMPLATE(plus,+)
ARITHEMTIC_OPERATION_TEMPLATE(minus,-)
ARITHEMTIC_OPERATION_TEMPLATE(division,/)
ARITHEMTIC_OPERATION_TEMPLATE(multiplication,*)

RELATION_OPERATION_TEMPLATE(equals,==)
RELATION_OPERATION_TEMPLATE(not_equals,!=)
RELATION_OPERATION_TEMPLATE(less_equal,<=)
RELATION_OPERATION_TEMPLATE(greater_equal,>=)
RELATION_OPERATION_TEMPLATE(less,<)
RELATION_OPERATION_TEMPLATE(greater,>)

const char operator_priority[] = {
    ['!'] = 11,
    ['k'] = 11, //unary +
    ['z'] = 11, //unary -
    ['*'] = 10,
    ['/'] = 10,
    ['%'] = 10,
    ['+'] = 9,
    ['-'] = 9,
    ['<'] = 7,
    ['>'] = 7,
    ['g'] = 7, //>=
    ['l'] = 7, //<=
    ['e'] = 6, //equal
    ['n'] = 6, //not equal
    ['&'] = 5,
    ['|'] = 4,
    ['a'] = 3, //and
    ['o'] = 2, //or
    [255] = 0
};

const char binary_operator_length[] = {
    ['*'] = 1,
    ['/'] = 1,
    ['%'] = 1,
    ['+'] = 1,
    ['-'] = 1,
    ['<'] = 1,
    ['>'] = 1,
    ['g'] = 2, //>=
    ['l'] = 2, //<=
    ['e'] = 2, //equal
    ['n'] = 2, //not equal
    ['&'] = 1,
    ['|'] = 1,
    ['a'] = 2, //and
    ['o'] = 2, //or
};

Variable (*binary_operations[])(Variable, Variable) = {
    ['+'] = operation_plus,
    ['-'] = operation_minus,
    ['/'] = operation_division,
    ['*'] = operation_multiplication,
    ['e'] = operation_equals,
    ['n'] = operation_not_equals,
    ['<'] = operation_less,
    ['>'] = operation_greater,
    ['g'] = operation_greater_equal,
    ['l'] = operation_less_equal,
    ['a'] = operation_and,
    ['o'] = operation_or
};

Variable (*unary_operations[])(Variable) = {
    ['-'] = operation_unary_minus,
    ['+'] = operation_unary_plus,
    ['!'] = operation_negation
};

const char symbol_map[] = {
    ['('] = ParenthesesLeft,
    [')'] = ParenthesesRight,
    ['a' ... 'z'] = Identifier,
    ['A' ... 'Z'] = Identifier,
    ['_'] = Identifier,
    ['0' ... '9'] = Number,
    ['.'] = FloatingPoint,
    [' '] = None,
    ['\t'] = None,
    ['+'] = Operator,
    ['-'] = Operator,
    ['/'] = Operator,
    ['*'] = Operator,
    ['>'] = Operator,
    ['<'] = Operator,
    ['='] = Operator,
    ['!'] = Operator,
    ['&'] = Operator,
    ['|'] = Operator,
    [255] = Invalid
};

static const char *get_parentheses_end(const char *left_parenthesis_start) {
    const char *p = left_parenthesis_start + 1;
    for(int parentheses = 1; *p; p++) {
        switch(*p) {
            case '(': 
                parentheses++;
                continue;
            case ')':
                parentheses--;
                if(parentheses < 0) {
                    goto error;
                }
            default:
                if(parentheses == 0) {
                    return p+1;
                }
        }
    }
    error:
    print_error("invalid parenthesis in %s", left_parenthesis_start);
}

static const char *get_next_operand_end_position(const char *start, const char *end) {
    enum Operand op = UnknownOperand;
    int parentheses = 0;
    const char *p = start;
    for(; p < end; p++) {
        switch(symbol_map[*p]) {
            case Identifier:
                if(op == IntOperand || op == FloatOperand) {
                    print_error("invalid operand in exp %s", start);
                }
                op = VariableOperand;
                break;
            case Number:
                if(op == UnknownOperand) {
                    op = IntOperand;
                }
                break;
            case FloatingPoint:
                if(op == UnknownOperand || op == IntOperand) {
                    op = FloatOperand;
                }
                else {
                    print_error("FloatingPoint invalid operand in exp %s", start);
                }
                break;
            case ParenthesesLeft:
                return get_parentheses_end(p);
            case ParenthesesRight:
                print_error("wrong parentheses %s", start);
            default:
                if(op == UnknownOperand) {
                    break;
                }
                return p;
        }
    }
    return p;
}

static inline char get_next_operator(const char *operand_end, const char *expr_end) {
    const char *p = operand_end;
    while(symbol_map[*p] == None && p < expr_end) p++; 
    if(p == expr_end) {
        return '\0';
    }
    switch(*p) {
        case '+':
        case '-':
        case '/':
        case '*':
            return *p;
        case '&':
            if(*++p == '&') {
                return 'a';
            }
            print_error("operator & not implemented");
        case '|':
            if(*++p == '|') {
                return 'o';
            }
            print_error("operator | not implemented");
        case '=':
            if(*++p == '=') {
                return 'e';
            }
            print_error("operator = not implemented");
        case '!':
            if(*++p == '=') {
                return 'n';
            }
            print_error("invalid expression \"%s\"", operand_end);
        case '>':
            if(*++p == '=') {
                return 'g';
            }
            return '>';
        case '<':
            if(*++p == '=') {
                return 'l';
            }
            return '<';
        case '\0':
            return '\0';
    }
    print_error("invalid expression \"%s\"", operand_end);
}

static const char *lowest_priority_op(const char *start, const char *end, char *op) {
    const char *p = start, *lowest = NULL;
    int lowest_priority = 999;
    while(1) {
        p = get_next_operand_end_position(p, end);
        char operator = get_next_operator(p, end);
        if(operator == '\0') {
            return lowest;
        }
        int priority = operator_priority[operator];
        if(priority != 0 && priority <= lowest_priority) {
            *op = operator;
            lowest = p;
            lowest_priority = priority;
        }
    } 
}

static Variable eval_number(const char *start, const char *end) {
    debug_print("evaluating number: ");
    print_expression(start, end);
    const char *p = start;
    enum Operand op = IntOperand;
    op = IntOperand;
    for(; p < end; p++) {
        switch(symbol_map[*p]) {
            case Number:
                continue;
            case FloatingPoint:
                if(op == IntOperand) {
                    op = FloatOperand;
                }
                else if(op == FloatOperand) {
                    print_error("more than on floatingpoint eval_number error [%s]", start);
                }
                continue;
            case None:
                continue;
            default:
                print_error("eval_number error [%s]", start);

        }
    }
    int l = end - start;
    debug_print("length: %d\n", l);
    char *num = malloc(l+1);
    strncpy(num, start, l);
    num[l] = '\0';
    debug_print("strncpy: \"%s\"\n", num);
    if(op == IntOperand) {
        int i = strtol(num, NULL, 10);
        debug_print("created int number: %d\n", i);
        return (Variable)integer(i);
    }
    float f = strtof(num,NULL);
    debug_print("created float number: %f\n", f);
    return (Variable){.type=Float, .f=f};
}

static Variable eval_identifier(MapObject *locals, const char *start, const char *end) {
    int l = end - start;
    char *key = malloc(l+1);
    strncpy(key, start, l);
    key[l] = '\0';
    Variable v = map_get(locals, key);
    free(key);
    return v;
}


static Variable eval_rec(MapObject *locals, const char *start, const char *end) {
    char operator = '\0';
    const char *lowest = lowest_priority_op(start, end, &operator);
    print_expression(start,end);
    if(lowest == NULL) {
        while(*start == ' ' || *start == '\t' || *start == '+') start++;
        switch(*start) {
            case '(': return eval_rec(locals, start+1, end-1);
            case '-': return operation_unary_minus(eval_rec(locals, start+1, end));
            case '!': return operation_negation(eval_rec(locals, start+1, end));
            default:
                switch(symbol_map[*start]) {
                    case Number:
                        return eval_number(start,end);
                    case Identifier:
                        return eval_identifier(locals,start,end);
                    default:
                        print_error("eval simple expression error");
                }
        }
    }
    Variable operand1 = eval_rec(locals, start, lowest);
    debug_print("operand2: ");
    print_expression(lowest, end);
    while(symbol_map[*lowest] == None && lowest < end) lowest++;
    Variable operand2 = eval_rec(locals, lowest + binary_operator_length[operator], end);
    Variable (*binop)(Variable, Variable) = binary_operations[operator];
    if(binop == NULL) {
        print_error("operator [ascii_code:%d] %c doesn't exist\n", operator, operator);
    }
    return binary_operations[operator](operand1, operand2);
}

Variable evaluate(MapObject *locals, const char* expr) {
    const char *end = expr + strlen(expr);
    return eval_rec(locals, expr, end);
}

void test_expr(const char *expr) {
    Variable v = evaluate(NULL, expr);
    if(v.type == Integer)
        printf("\n[%s]: %d\n", expr, v.i);
    else if(v.type == Float)
        printf("\n[%s]: %f\n", expr, v.f);
    else
        printf("unsupported yet 565");
}

void eval_testing() {
    //test_expr("    \t\t   (   1     +    1  )     ");
    test_expr("1 > 1");
    test_expr("884 == 666");
    test_expr("884 != 666");
    test_expr(" 39 || 48");
    test_expr(" 0 && 48");
    test_expr(" 1 && 48");
    test_expr("3*35325/325-10 + 23 +44-34-32+-34*35&&44 ");
    test_expr("(32+-(34.0*35))");
    test_expr("((3*35325/325)-10 + 23) +(44-34)-(32+-(34.0*35))");
}
