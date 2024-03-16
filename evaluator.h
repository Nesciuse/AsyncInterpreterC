#ifndef H_EVALUATOR
#define H_EVALUATOR

#include "variable_type.h"
#include "map.h"

#define EVALUATOR_DEBUG 0
#define EVALUATOR_PRINT_EXPRESSION 0

#define ARITHEMTIC_OPERATION_TEMPLATE(name,op)\
    Variable operation_##name (Variable a, Variable b) {\
        switch(a.type) {\
            case Integer:\
                switch(b.type) {\
                    case Integer: return (Variable)integer(a.i op b.i);\
                    case Float: return (Variable){.type=Float, .f=(a.i op b.f)};\
                }\
                break;\
            case Float:\
                switch(b.type) {\
                    case Integer: return (Variable){.type=Float, .f=(a.f op b.i)};\
                    case Float: return (Variable){.type=Float, .f=(a.f op b.f)};\
                }\
        }\
        eval_error(a, b, #op);\
    }\

#define RELATION_OPERATION_TEMPLATE(name,op)\
    Variable operation_##name (Variable a, Variable b) {\
        switch(a.type) {\
            case Integer:\
                switch(b.type) {\
                    case Integer: return (Variable)integer(a.i op b.i);\
                    case Float: return (Variable)integer(a.i op b.f);\
                }\
                break;\
            case Float:\
                switch(b.type) {\
                    case Integer: return (Variable)integer(a.f op b.i);\
                    case Float: return (Variable)integer(a.f op b.f);\
                }\
        }\
        eval_error(a, b, #op);\
    }\

#define UNARY_OPERATION_TEMPLATE(name,op)\
    Variable operation_##name (Variable a) {\
        switch(a.type) {\
            case Integer: return (Variable)integer(op a.i);\
            case Float: return (Variable){.type=Float, .f=op a.f};\
        }\
        eval_error(a, a, #op);\
    }\

enum {
    Invalid = 0,
    Identifier,
    Vague,
    Number,
    BinaryOperator,
    BinaryOperator_p1,
    UnaryOperator,
    ParenthesesLeft,
    ParenthesesRight,
    StringSymbol,
    FloatingPoint,
    Operator,
    None
};

enum Operand {
    IntOperand = 0,
    FloatOperand,
    ParenthesisOperand,
    VariableOperand,
    UnknownOperand
};

Variable evaluate(MapObject *locals, const char* expr);
void eval_testing();

#endif