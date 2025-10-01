#include "My_stack.h"

errno_t My_stack_Ctor(My_stack *const stack_ptr, size_t const start_capacity
                      ON_DEBUG(, Var_info const var_info)) {
    assert(stack_ptr); assert(!stack_ptr->is_valid); assert(start_capacity);
    ON_DEBUG(assert(var_info.position.file_name); assert(var_info.name);)

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t *const new_buffer = (stack_elem_t *)calloc(start_capacity + 2 * CANARY_NUM,
                                                            sizeof(stack_elem_t));

    if (!new_buffer) {
        PRINT_LINE();
        perror("calloc failed");
        CLEAR_RESOURCES();
        return errno;
    }

    for (size_t i = 0; i < CANARY_NUM; ++i) {
        stack_ptr->beg_canary[i] = CANARY;
    }

    stack_ptr->size              = 0;
    stack_ptr->capacity          = start_capacity;
    stack_ptr->buffer            = new_buffer + CANARY_NUM;
    ON_DEBUG(stack_ptr->var_info = var_info;)
    stack_ptr->is_valid          = true;

    for (size_t i = 0; i < CANARY_NUM; ++i) {
        stack_ptr->end_canary[i] = CANARY;
    }

    for (size_t i = 0; i < CANARY_NUM; ++i) {
        (stack_ptr->buffer - CANARY_NUM)         [i] = BUFFER_CANARY;
    }
    for (size_t i = 0; i < CANARY_NUM; ++i) {
        (stack_ptr->buffer + stack_ptr->capacity)[i] = BUFFER_CANARY;
    }

    CLEAR_RESOURCES();
    return 0;
}

void My_stack_Dtor(My_stack *const stack_ptr) {
    assert(stack_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    free(stack_ptr->buffer - CANARY_NUM);

    CLEAR_RESOURCES();
}

errno_t My_stack_verify(My_stack const *const stack_ptr) {
    assert(stack_ptr);

    errno_t err = 0;
#undef FINAL_CODE
#define FINAL_CODE

    for (size_t i = 0; i < CANARY_NUM; ++i) {
        if (stack_ptr->beg_canary[i] != CANARY or
            stack_ptr->end_canary[i] != CANARY) {
            err |= STACK_CANARY_SPOILED;
            break;
        }
    }

    if (!stack_ptr->is_valid) {
        err |= STACK_INVALID;
    }

    if (!stack_ptr->capacity) {
        err |= STACK_NULL_CAPACITY;
    }

    if (stack_ptr->size > stack_ptr->capacity) {
        err |= STACK_SIZE_GREATER_THAN_CAPACITY;
    }

    if (!stack_ptr->buffer) {
        err |= STACK_NULL_BUFFER;
    }

    for (size_t i = 0; i < CANARY_NUM; ++i) {
        if ((stack_ptr->buffer - CANARY_NUM)         [i] != BUFFER_CANARY or
            (stack_ptr->buffer + stack_ptr->capacity)[i] != BUFFER_CANARY) {
            err |= STACK_BUFFER_CANARY_SPOILED;
        }
    }

    CLEAR_RESOURCES();
    return err;
}

void My_stack_dump(FILE *const out_stream, My_stack const *const stack_ptr,
                   Position_info const from_where, errno_t const err) {
    assert(out_stream); assert(stack_ptr);
    ON_DEBUG(assert(stack_ptr->var_info.name); assert(stack_ptr->var_info.position.file_name);)
    assert(from_where.file_name);

#undef FINAL_CODE
#define FINAL_CODE

    fprintf_s(out_stream, "called at file %s, line %d in \"%s\" function: ",
              from_where.file_name, from_where.line, from_where.function_name);

    if (err & STACK_CANARY_SPOILED) {
        fprintf_s(out_stream, "Canary spoiled    "); //TODO - I can't use \t there
    }

    if (err & STACK_INVALID) {
        fprintf_s(out_stream, "Stack is invalid    ");
    }

    if (err & STACK_NULL_CAPACITY) {
        fprintf_s(out_stream, "Stack has null capacity    ");
    }

    if (err & STACK_SIZE_GREATER_THAN_CAPACITY) {
        fprintf_s(out_stream, "Stack size greater, than capacity    ");
    }

    if (err & STACK_NULL_BUFFER) {
        fprintf_s(out_stream, "Stack has null buffer    ");
    }

    if (err & STACK_BUFFER_CANARY_SPOILED) {
        fprintf_s(out_stream, "Stack buffer canary spoiled    ");
    }

    fprintf_s(out_stream, "\nstack<%s>[%p]"
              ON_DEBUG(" \"%s\" declared in file %s, line %zu in \"%s\" function")
              " {\n",
              stack_elem_str, stack_ptr
              ON_DEBUG(, stack_ptr->var_info.name,
              stack_ptr->var_info.position.file_name, stack_ptr->var_info.position.line,
              stack_ptr->var_info.position.function_name));

    fprintf_s(out_stream, "\tbeg_canary = [%p] {\n", stack_ptr->beg_canary);
    for (size_t i = 0; i < CANARY_NUM; ++i) {
        fprintf_s(out_stream, "\t\t[%zu] = [%#zX]\n", i, stack_ptr->beg_canary[i]);
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "\tsize = %zu\n",     stack_ptr->size);
    fprintf_s(out_stream, "\tcapacity = %zu\n", stack_ptr->capacity);

    fprintf_s(out_stream, "\tbuffer[%zu] = [%p] {\n", stack_ptr->capacity, stack_ptr->buffer);
    for (size_t i = 0; i < CANARY_NUM; ++i) {
        fprintf_s(out_stream, "\t\t[BEG_CANARY] = ");
        fprintf_s(out_stream, stack_canary_frm, (stack_ptr->buffer - CANARY_NUM)[i]);
        fprintf_s(out_stream, "\n");
    }
    for (size_t i = 0; i < stack_ptr->size; ++i) {
        fprintf_s(out_stream, "\t\t[%zu] = ", i);
        fprintf_s(out_stream, stack_elem_frm, stack_ptr->buffer[i]);
        fprintf_s(out_stream, "\n");
    }
    for (size_t i = stack_ptr->size; i < stack_ptr->capacity; ++i) {
        fprintf_s(out_stream, "\t\t[%zu] = ", i);
        fprintf_s(out_stream, stack_elem_frm, stack_ptr->buffer[i]);
        fprintf_s(out_stream, "(Out of range)\n");
    }
    for (size_t i = 0; i < CANARY_NUM; ++i) {
        fprintf_s(out_stream, "\t\t[END_CANARY] = ");
        fprintf_s(out_stream, stack_canary_frm, (stack_ptr->buffer + stack_ptr->capacity)[i]);
        fprintf_s(out_stream, "\n");
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "\tis_valid = %d\n", stack_ptr->is_valid);

    fprintf_s(out_stream, "\tend_canary = [%p] {\n", stack_ptr->end_canary);
    for (size_t i = 0; i < CANARY_NUM; ++i) {
        fprintf_s(out_stream, "\t\t[%zu] = [%#zX]\n", i, stack_ptr->end_canary[i]);
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "}\n");
    CLEAR_RESOURCES();
}

errno_t My_stack_push(My_stack *const stack_ptr, stack_elem_t const elem) {
    assert(stack_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    ON_DEBUG(CHECK_FUNC(My_stack_verify, stack_ptr);)
    if (stack_ptr->size == stack_ptr->capacity) {
        stack_elem_t *const new_buffer = (stack_elem_t *)realloc(stack_ptr->buffer - 1,
                                                                 (2 * stack_ptr->capacity +
                                                                 2 * CANARY_NUM) * sizeof(stack_elem_t));
        if (!new_buffer) {
            PRINT_LINE();
            perror("realloc failed");
            CLEAR_RESOURCES();
            return errno;
        }

        stack_ptr->capacity *= 2;
        stack_ptr->buffer = new_buffer + CANARY_NUM;

        for (size_t i = 0; i < CANARY_NUM; ++i) {
            (stack_ptr->buffer - CANARY_NUM)         [i] = BUFFER_CANARY;
        }
        for (size_t i = 0; i < CANARY_NUM; ++i) {
            (stack_ptr->buffer + stack_ptr->capacity)[i] = BUFFER_CANARY;
        }
    }

    stack_ptr->buffer[stack_ptr->size++] = elem;
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

    if (stack_ptr->size < stack_ptr->capacity / 4)
    {
        stack_elem_t *const new_buffer = (stack_elem_t *)realloc(stack_ptr->buffer - 1,
                                                                 (stack_ptr->capacity / 2 +
                                                                 2 * CANARY_NUM) * sizeof(stack_elem_t));
        if (!new_buffer) {
            PRINT_LINE();
            perror("realloc failed");
            CLEAR_RESOURCES();
            return errno;
        }

        stack_ptr->capacity /= 2;
        stack_ptr->buffer = new_buffer + CANARY_NUM;

        for (size_t i = 0; i < CANARY_NUM; ++i) {
            (stack_ptr->buffer - CANARY_NUM)         [i] = BUFFER_CANARY;
        }
        for (size_t i = 0; i < CANARY_NUM; ++i) {
            (stack_ptr->buffer + stack_ptr->capacity)[i] = BUFFER_CANARY;
        }
    }

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
