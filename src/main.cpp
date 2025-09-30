#include "Common.h"
#include "My_stack.h"

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

#undef FINAL_CODE
#define FINAL_CODE

    STACK_CREATE(stk, 1);
#undef FINAL_CODE
#define FINAL_CODE My_stack_Dtor(&stk);

    STACK_DUMP(stderr, stk, 0);
    My_stack_pop(&stk, nullptr);
    My_stack_push(&stk, 4);
    My_stack_push(&stk, 1);
    My_stack_push(&stk, 10);
    STACK_DUMP(stderr, stk, 0B111111);
    stack_elem_t tp = 0;
    My_stack_top(&stk, &tp);
    My_stack_pop(&stk, nullptr);
    My_stack_pop(&stk, nullptr);
    STACK_DUMP(stderr, stk, 0);
    My_stack_pop(&stk, nullptr);
    STACK_DUMP(stderr, stk, 0);

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
