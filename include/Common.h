#ifndef COMMON_H
#define COMMON_H

#include "Colored_printf.h"
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#define PRINT_LINE() fprintf(stderr, "Error found, file " __FILE__ ", line %d\n", __LINE__)

#define CLEAR_RESOURCES()   \
do                          \
{                           \
    FINAL_CODE              \
} while (false)

#define CHECK_FUNC(func, ...)       \
do                                  \
{                                   \
    if (func(__VA_ARGS__))          \
    {                               \
        PRINT_LINE();               \
        perror(#func " failed");    \
        CLEAR_RESOURCES();          \
        return errno;               \
    }                               \
} while (false)

//TODO - is it necessary?
#define MAIN_CHECK_FUNC(func, ...)  \
do                                  \
{                                   \
    if (func(__VA_ARGS__))          \
    {                               \
        PRINT_LINE();               \
        perror(#func " failed");    \
        CLEAR_RESOURCES();          \
        return 0;                   \
    }                               \
} while (false)

#endif
