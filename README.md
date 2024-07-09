Unfinished project

# Async Interpreter C
Theme: Asynchronous interpreter

Annotation: In asynchronous programming various asynchronous operations are usually executed inside a main loop. An asynchronous function is then a function that, in addition to synchronous operations, can contain a number of asynchronous ones. Execution of an asynchronous operation means that the function needs to be suspend while waiting for the operation to finish. In the meantime the main loop can execute other operations and functions. When the given operation finishes, the encompassing function needs to be resumed in the correct position.

Goal: Design and implement an interpreter of simple but asynchronous instructions.
Instructions should include ones that allow working with variables, control flow statements and procedure / function calls.
It should be easy to integrate the interpreter into different C/C++ projects with asynchronous design.
