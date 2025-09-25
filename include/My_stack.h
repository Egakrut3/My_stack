#ifndef MY_STACK_H
#define MY_STACK_H

#include "Common.h"

typedef int stack_elem_t;
char const str[] = "int";

struct My_stack
{
    size_t size,
           capacity;
    stack_elem_t *buffer;

    ON_DEBUG(
};



#endif
