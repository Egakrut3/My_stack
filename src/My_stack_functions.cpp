#include "My_stack.h"

errno_t My_stack_push(My_stack *const stack_ptr, stack_elem_t const elem) {
    assert(stack_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    ON_DEBUG(CHECK_FUNC(My_stack_verify, stack_ptr);)
    if (stack_ptr->size == stack_ptr->capacity) {
        stack_elem_t *const new_buffer = (stack_elem_t *)realloc(stack_ptr->buffer - STACK_CANARY_NUM,
                                                                 (STACK_EXPANSION * stack_ptr->capacity +
                                                                 2 * STACK_CANARY_NUM) * sizeof(stack_elem_t));
        if (!new_buffer) {
            PRINT_LINE();
            perror("realloc failed");
            CLEAR_RESOURCES();
            return errno;
        }

        stack_ptr->capacity *= STACK_EXPANSION;
        stack_ptr->buffer            = new_buffer + STACK_CANARY_NUM;

        for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
            (stack_ptr->buffer - STACK_CANARY_NUM)[i] = BUFFER_CANARY;
        }
        for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
            (stack_ptr->buffer + stack_ptr->capacity)[i] = BUFFER_CANARY;
        }
    }

    stack_ptr->buffer[stack_ptr->size++] = elem;
    ON_DEBUG(stack_ptr->hash_val = My_stack_hash(stack_ptr);)
    CLEAR_RESOURCES();
    return 0;
}

errno_t My_stack_pop(My_stack *const stack_ptr, stack_elem_t *const dest) {
    assert(stack_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    ON_DEBUG(CHECK_FUNC(My_stack_verify, stack_ptr);)
    if (!stack_ptr->size) {
        PRINT_LINE();
        fprintf_s(stderr, "Stack is empty before pop operation\n");
        CLEAR_RESOURCES();
        return ATTEMPT_TO_ACCESS_TOP_FROM_EMPTY;
    }

    --stack_ptr->size;
    if (dest) {
        *dest = stack_ptr->buffer[stack_ptr->size];
    }

    if (stack_ptr->size < stack_ptr->capacity / STACK_REDUCTION_CRITERIA and
        stack_ptr->capacity / STACK_REDUCTION >= STACK_MIN_CAPACITY)
    {
        stack_elem_t *const new_buffer = (stack_elem_t *)realloc(stack_ptr->buffer - STACK_CANARY_NUM,
                                                                 (stack_ptr->capacity / STACK_REDUCTION +
                                                                 2 * STACK_CANARY_NUM) * sizeof(stack_elem_t));
        if (!new_buffer) {
            PRINT_LINE();
            perror("realloc failed");
            CLEAR_RESOURCES();
            return errno;
        }

        stack_ptr->capacity /= STACK_REDUCTION;
        stack_ptr->buffer            = new_buffer + STACK_CANARY_NUM;

        for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
            (stack_ptr->buffer - STACK_CANARY_NUM)[i] = BUFFER_CANARY;
        }
        for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
            (stack_ptr->buffer + stack_ptr->capacity)[i] = BUFFER_CANARY;
        }
    }

    ON_DEBUG(stack_ptr->hash_val = My_stack_hash(stack_ptr);)
    CLEAR_RESOURCES();
    return 0;
}

errno_t My_stack_top(My_stack const *const stack_ptr, stack_elem_t *const dest) {
    assert(stack_ptr); assert(dest);

#undef FINAL_CODE
#define FINAL_CODE

    ON_DEBUG(CHECK_FUNC(My_stack_verify, stack_ptr);)
    if (!stack_ptr->size) {
        PRINT_LINE();
        fprintf_s(stderr, "Stack is empty before top operation\n");
        CLEAR_RESOURCES();
        return ATTEMPT_TO_ACCESS_TOP_FROM_EMPTY;
    }

    *dest = stack_ptr->buffer[stack_ptr->size - 1];
    CLEAR_RESOURCES();
    return 0;
}
