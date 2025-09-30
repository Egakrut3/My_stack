#include "My_stack.h"

static void fill_canary(void *const ptr, size_t const num) {
    assert(ptr);

#undef FINAL_CODE
#define FINAL_CODE

    char *const end_ptr = (char *)ptr + num * CANARY_SIZE;
    for (char *ch_ptr = (char *)ptr; ch_ptr != end_ptr; ch_ptr += CANARY_SIZE) {
        for (size_t i = 0; i < CANARY_SIZE; ++i) {
            ch_ptr[i] = CANARY_PARTS[i];
        }
    }

    CLEAR_RESOURCES();
}

static bool check_canary(void const *const ptr, size_t const num) {
    assert(ptr);

#undef FINAL_CODE
#define FINAL_CODE

    char const *const end_ptr = (char const *)ptr + num * CANARY_SIZE;
    for (char const *ch_ptr = (char const *)ptr; ch_ptr != end_ptr; ch_ptr += CANARY_SIZE) {
        for (size_t i = 0; i < CANARY_SIZE; ++i) {
            if (ch_ptr[i] != CANARY_PARTS[i]) {
                return true;
            }
        }
    }

    CLEAR_RESOURCES();
    return false;
}

errno_t My_stack_Ctor(My_stack *const stack_ptr, size_t const start_capacity
                      ON_DEBUG(, Var_info const var_info)) {
    assert(stack_ptr); assert(!stack_ptr->is_valid); assert(start_capacity);
    ON_DEBUG(assert(var_info.position.file_name); assert(var_info.name);)

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t *const new_buffer = (stack_elem_t *)calloc(start_capacity +
                                                            2 * CANARY_NUM * CANARY_SIZE,
                                                            sizeof(stack_elem_t));
    if (!new_buffer) {
        PRINT_LINE();
        perror("calloc failed");
        CLEAR_RESOURCES();
        return errno;
    }

    stack_ptr->size              = 0;
    stack_ptr->capacity          = start_capacity;
    stack_ptr->buffer            = new_buffer + CANARY_SIZE;
    ON_DEBUG(stack_ptr->var_info = var_info;)
    stack_ptr->is_valid          = true;

    fill_canary(stack_ptr->canary_beg,                   CANARY_NUM);
    fill_canary(stack_ptr->canary_end,                   CANARY_NUM);
    fill_canary(new_buffer,                              CANARY_NUM * sizeof(stack_elem_t));
    fill_canary(stack_ptr->buffer + stack_ptr->capacity, CANARY_NUM * sizeof(stack_elem_t));

    CLEAR_RESOURCES();
    return 0;
}

void My_stack_Dtor(My_stack *const stack_ptr) {
    assert(stack_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    free(stack_ptr->buffer - CANARY_SIZE);

    CLEAR_RESOURCES();
}

errno_t My_stack_verify(My_stack const *const stack_ptr) {
    assert(stack_ptr);

    errno_t err = 0;
#undef FINAL_CODE
#define FINAL_CODE

    if (check_canary(stack_ptr->canary_beg, CANARY_NUM) or
        check_canary(stack_ptr->canary_end, CANARY_NUM)) {
        err |= STACK_CANARY_SPOILED;
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

    if (check_canary(stack_ptr->buffer - CANARY_SIZE,         CANARY_NUM * sizeof(stack_elem_t)) or
        check_canary(stack_ptr->buffer + stack_ptr->capacity, CANARY_NUM * sizeof(stack_elem_t))) {
        err |= STACK_BUFFER_CANARY_SPOILED;
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

    fprintf_s(out_stream, "\tcanary_beg[%zu][%zu] = [%p] {\n",
              CANARY_NUM, CANARY_SIZE, stack_ptr->canary_beg);
    for (size_t i = 0; i < CANARY_NUM; ++i) {
        for (size_t j = 0; j < CANARY_SIZE; ++j) {
            fprintf_s(out_stream, "\t\t[%zu][%zu] = %#hhX\n", i, j, stack_ptr->canary_beg[i][j]);
        }
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "\tsize = %zu\n", stack_ptr->size);
    fprintf_s(out_stream, "\tcapacity = %zu\n", stack_ptr->capacity);

    char const *const buffer_canary_beg = (char *)(stack_ptr->buffer - CANARY_SIZE);
    fprintf_s(out_stream, "\tbuffer_canary_beg[%zu][%zu] = [%p] {\n",
              CANARY_NUM * sizeof(stack_elem_t), CANARY_SIZE, buffer_canary_beg);
    for (size_t i = 0; i < CANARY_NUM * sizeof(stack_elem_t); ++i) {
        for (size_t j = 0; j < CANARY_SIZE; ++j) {
            fprintf_s(out_stream, "\t\t[%zu][%zu] = %#hhX\n",
                      i, j, buffer_canary_beg[i * CANARY_SIZE + j]);
        }
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "\tbuffer[%zu] = [%p] {\n", stack_ptr->capacity, stack_ptr->buffer);
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
    fprintf_s(out_stream, "\t}\n");

    char const *const buffer_canary_end = (char *)(stack_ptr->buffer + stack_ptr->capacity);
    fprintf_s(out_stream, "\tbuffer_canary_end[%zu][%zu] = [%p] {\n",
              CANARY_NUM * sizeof(stack_elem_t), CANARY_SIZE, buffer_canary_end);
    for (size_t i = 0; i < CANARY_NUM * sizeof(stack_elem_t); ++i) {
        for (size_t j = 0; j < CANARY_SIZE; ++j) {
            fprintf_s(out_stream, "\t\t[%zu][%zu] = %#hhX\n",
                      i, j, buffer_canary_end[i * CANARY_SIZE + j]);
        }
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "\tis_valid = %d\n", stack_ptr->is_valid);

    fprintf_s(out_stream, "\tcanary_end[%zu][%zu] = [%p] {\n",
              CANARY_NUM, CANARY_SIZE, stack_ptr->canary_end);
    for (size_t i = 0; i < CANARY_NUM; ++i) {
        for (size_t j = 0; j < CANARY_SIZE; ++j) {
            fprintf_s(out_stream, "\t\t[%zu][%zu] = %#hhX\n", i, j, stack_ptr->canary_end[i][j]);
        }
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "}\n");
    CLEAR_RESOURCES();
}

errno_t My_stack_push(My_stack *const stack_ptr, stack_elem_t const elem) {
    assert(stack_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    CHECK_FUNC(My_stack_verify, stack_ptr);
    if (stack_ptr->size == stack_ptr->capacity) {
        stack_elem_t *const new_buffer = (stack_elem_t *)realloc(stack_ptr->buffer - CANARY_SIZE,
                                                                 (2 * stack_ptr->capacity +
                                                                 2 * CANARY_NUM * CANARY_SIZE) *
                                                                 sizeof(stack_elem_t));
        if (!new_buffer) {
            PRINT_LINE();
            perror("realloc failed");
            CLEAR_RESOURCES();
            return errno;
        }

        stack_ptr->capacity *= 2;
        stack_ptr->buffer = new_buffer + CANARY_SIZE;

        fill_canary(new_buffer,                              CANARY_NUM * sizeof(stack_elem_t));
        fill_canary(stack_ptr->buffer + stack_ptr->capacity, CANARY_NUM * sizeof(stack_elem_t));
    }

    stack_ptr->buffer[stack_ptr->size++] = elem;
    CLEAR_RESOURCES();
    return 0;
}

errno_t My_stack_pop(My_stack *const stack_ptr, stack_elem_t *const dest) {
    assert(stack_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    CHECK_FUNC(My_stack_verify, stack_ptr);
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
        stack_elem_t *const new_buffer = (stack_elem_t *)realloc(stack_ptr->buffer - CANARY_SIZE,
                                                                 (stack_ptr->capacity / 2 +
                                                                 2 * CANARY_NUM * CANARY_SIZE) *
                                                                 sizeof(stack_elem_t));
        if (!new_buffer) {
            PRINT_LINE();
            perror("realloc failed");
            CLEAR_RESOURCES();
            return errno;
        }

        stack_ptr->capacity /= 2;
        stack_ptr->buffer = new_buffer + CANARY_SIZE;

        fill_canary(new_buffer,                              CANARY_NUM * sizeof(stack_elem_t));
        fill_canary(stack_ptr->buffer + stack_ptr->capacity, CANARY_NUM * sizeof(stack_elem_t));
        CHECK_FUNC(My_stack_verify, stack_ptr);
    }

    CLEAR_RESOURCES();
    return 0;
}

errno_t My_stack_top(My_stack const *const stack_ptr, stack_elem_t *const dest) {
    assert(stack_ptr); assert(dest);

#undef FINAL_CODE
#define FINAL_CODE

    CHECK_FUNC(My_stack_verify, stack_ptr);

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
