#include "Common.h"
#include "My_stack.h"

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

    #undef FINAL_CODE
    #define FINAL_CODE

    STACK_CREATE(stk, 1, MAIN_CHECK_FUNC);
    #undef FINAL_CODE
    #define FINAL_CODE          \
        My_stack_Dtor(&stk);

    STACK_DUMP(stderr, stk, 0, MAIN_CHECK_FUNC);
    My_stack_pop(&stk, nullptr);
    My_stack_push(&stk, 4);
    STACK_DUMP(stderr, stk, 0, MAIN_CHECK_FUNC);
    My_stack_push(&stk, 1);
    My_stack_push(&stk, 10);
    STACK_DUMP(stderr, stk, 0B111111, MAIN_CHECK_FUNC);
    stack_elem_t tp = 0;
    My_stack_top(&stk, &tp);
    fprintf_s(stderr, STACK_ELEM_FRM, tp);
    fprintf_s(stderr, "\n");
    My_stack_pop(&stk, nullptr);
    My_stack_pop(&stk, nullptr);
    STACK_DUMP(stderr, stk, 0, MAIN_CHECK_FUNC);
    My_stack_pop(&stk, nullptr);
    STACK_DUMP(stderr, stk, 0, MAIN_CHECK_FUNC);

    char *const ptr = (char *)(&stk);
    *ptr = 0;
    *(ptr + 8) = 0;
    STACK_DUMP(stderr, stk, My_stack_verify(&stk), MAIN_CHECK_FUNC);

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
