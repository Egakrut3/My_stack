#include "Common.h"
#include "My_stack.h"

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

#undef FINAL_CODE
#define FINAL_CODE

    STACK_CREATE(stk, 1);
    STACK_DUMP(stderr, stk, 0B111111);

    colored_printf(RED, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
