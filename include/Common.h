#ifndef COMMON_H
#define COMMON_H

#include "Colored_printf.h"
#include <errno.h>
#include <assert.h>

#define __PRINT_LINE__() fprintf(stderr, "Error found, file " __FILE__ ", line %d\n", __LINE__)

#ifdef _DEBUG //TODO - _STACK_DEBUG

#define ON_DEBUG(...) __VA_ARGS__

#else

#define ON_DEBUG(...)

#endif

struct Var_info
{
    char const *var_name,
               *creator_function,
               *creator_file;
    ssize_t creator_line;
};

errno_t Var_info_ctor(Var_info *var_info_ptr, char const *var_name,
                                              char const *creator_function,
                                              char const *creator_file,
                                              ssize_t creator_line);

errno_t Var_info_ctor
