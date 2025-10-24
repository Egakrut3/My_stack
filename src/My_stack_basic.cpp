#include "My_stack.h"

uint64_t My_stack_hash(My_stack const *const stack_ptr) {
    assert(stack_ptr);

    uint64_t cur_hash = STACK_START_HASH;
    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        (uint64_t)stack_ptr->var_info.position.file_name;)
    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        (uint64_t)stack_ptr->var_info.position.function_name;)
    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        stack_ptr->var_info.position.line;)
    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        (uint64_t)stack_ptr->var_info.name;)

    cur_hash = cur_hash * STACK_HASH_MLT + stack_ptr->size;
    cur_hash = cur_hash * STACK_HASH_MLT + stack_ptr->capacity;
    cur_hash = cur_hash * STACK_HASH_MLT + (uint64_t)stack_ptr->buffer;
    for (size_t i = 0; i < stack_ptr->size; ++i) {
        cur_hash = cur_hash * STACK_HASH_MLT + *(uint64_t *)&stack_ptr->buffer[i];
    }
    cur_hash = cur_hash * STACK_HASH_MLT + stack_ptr->is_valid;

    return cur_hash;
}

#undef FINAL_CODE
#define FINAL_CODE

errno_t My_stack_Ctor(My_stack *const stack_ptr, size_t start_capacity
                      ON_DEBUG(, Var_info const var_info)) {
    assert(stack_ptr); assert(!stack_ptr->is_valid); assert(start_capacity);
    ON_DEBUG(assert(var_info.position.file_name); assert(var_info.name);)

    if (start_capacity < STACK_MIN_CAPACITY) {
        start_capacity = STACK_MIN_CAPACITY;
    }

    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
        stack_ptr->beg_canary[i] = CANARY;
    }

    ON_DEBUG(stack_ptr->var_info = var_info;)
    stack_ptr->size              = 0;
    stack_ptr->capacity          = start_capacity;

    CHECK_FUNC(My_calloc, (void **)&stack_ptr->buffer, start_capacity + 2 * STACK_CANARY_NUM, sizeof(stack_elem_t));
    stack_ptr->buffer += STACK_CANARY_NUM;

    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
        (stack_ptr->buffer - STACK_CANARY_NUM)[i] = STACK_BUFFER_CANARY;
    }
    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
        (stack_ptr->buffer + stack_ptr->capacity)[i] = STACK_BUFFER_CANARY;
    }

    stack_ptr->is_valid          = true;
    ON_DEBUG(stack_ptr->hash_val = My_stack_hash(stack_ptr);)

    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
        stack_ptr->end_canary[i] = CANARY;
    }

    return 0;
}

void My_stack_Dtor(My_stack *const stack_ptr) {
    assert(stack_ptr);

    free(stack_ptr->buffer - STACK_CANARY_NUM);
    stack_ptr->is_valid = false;
}

errno_t My_stack_verify(My_stack const *const stack_ptr) {
    assert(stack_ptr);

    errno_t err = 0;

    if (stack_ptr->hash_val != My_stack_hash(stack_ptr)) {
        err |= STACK_HASH_UNMATCH;
    }

    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
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

    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        if ((stack_ptr->buffer - STACK_CANARY_NUM)[i]    != STACK_BUFFER_CANARY or
            (stack_ptr->buffer + stack_ptr->capacity)[i] != STACK_BUFFER_CANARY) {
            err |= STACK_BUFFER_CANARY_SPOILED;
        }
        #pragma GCC diagnostic pop
    }

    return err;
}

errno_t My_stack_dump(FILE *const out_stream, My_stack const *const stack_ptr,
                      errno_t const err, Position_info const from_where,
                      size_t const tab_count, bool const need_extra_info) {
    assert(out_stream); assert(stack_ptr);
    ON_DEBUG(assert(stack_ptr->var_info.name); assert(stack_ptr->var_info.position.file_name);)
    assert(from_where.file_name);

    #undef FINAL_CODE
    #define FINAL_CODE

    char *tab_str = nullptr;
    CHECK_FUNC(My_calloc, (void **)&tab_str, tab_count + 1, sizeof(*tab_str));
    for (size_t i = 0; i < tab_count; ++i) {
        tab_str[i] = '\t';
    }
    #undef FINAL_CODE
    #define FINAL_CODE  \
        free(tab_str);

    if (need_extra_info) {
        fprintf_s(out_stream, "%scalled at file %s, line %d in \"%s\" function: ", tab_str,
                  from_where.file_name, from_where.line, from_where.function_name);

        if (err & STACK_HASH_UNMATCH) {
            fprintf_s(out_stream, "Stack hash unmatch    ");
        }

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

        fprintf_s(out_stream, "\n%sstack<%s>[%p]"
                 ON_DEBUG(" \"%s\" declared in file %s, line %zu in \"%s\" function")
                 " {\n", tab_str,
                 stack_elem_str, stack_ptr
                 ON_DEBUG(, stack_ptr->var_info.name,
                 stack_ptr->var_info.position.file_name, stack_ptr->var_info.position.line,
                 stack_ptr->var_info.position.function_name));
    }

    fprintf_s(out_stream, "%s\tbeg_canary[%zu] = [%p] {\n", tab_str,
              STACK_CANARY_NUM, stack_ptr->beg_canary);
    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
        fprintf_s(out_stream, "%s\t\t[%zu] = [%zX]\n", tab_str, i, stack_ptr->beg_canary[i]);
    }
    fprintf_s(out_stream, "%s\t}\n", tab_str);

    fprintf_s(out_stream, "%s\tsize = %zu\n",     tab_str, stack_ptr->size);
    fprintf_s(out_stream, "%s\tcapacity = %zu\n", tab_str, stack_ptr->capacity);

    fprintf_s(out_stream, "%s\tbuffer[%zu] = [%p] {\n", tab_str, stack_ptr->capacity, stack_ptr->buffer);
    if (stack_ptr->buffer) {
        for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
            fprintf_s(out_stream, "%s\t\t[BEG_CANARY] = " STACK_CANARY_FRM "\n", tab_str,
                      (stack_ptr->buffer - STACK_CANARY_NUM)[i]);
        }
        for (size_t i = 0; i < stack_ptr->size; ++i) {
            fprintf_s(out_stream, "%s\t\t[%zu] = " STACK_ELEM_FRM "\n", tab_str,
                      i, stack_ptr->buffer[i]);
        }
        for (size_t i = stack_ptr->size; i < stack_ptr->capacity; ++i) {
            fprintf_s(out_stream, "%s\t\t[%zu] = " STACK_ELEM_FRM "(Out of range)\n", tab_str,
                      i, stack_ptr->buffer[i]);
        }
        for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
            fprintf_s(out_stream, "%s\t\t[END_CANARY] = " STACK_CANARY_FRM "\n", tab_str,
                      (stack_ptr->buffer + stack_ptr->capacity)[i]);
        }
        fprintf_s(out_stream, "%s\t}\n", tab_str);
    }
    else {
        fprintf_s(out_stream, "%s\tInvalid buffer\n", tab_str);
    }

    ON_DEBUG(fprintf_s(out_stream, "%s\thash_val = %llX, must be %llX\n", tab_str,
                       stack_ptr->hash_val, My_stack_hash(stack_ptr)));
    fprintf_s(out_stream, "%s\tis_valid = %d\n", tab_str, stack_ptr->is_valid);

    fprintf_s(out_stream, "%s\tend_canary[%zu] = [%p] {\n", tab_str,
              STACK_CANARY_NUM, stack_ptr->end_canary);
    for (size_t i = 0; i < STACK_CANARY_NUM; ++i) {
        fprintf_s(out_stream, "%s\t\t[%zu] = [%zX]\n", tab_str, i, stack_ptr->end_canary[i]);
    }
    fprintf_s(out_stream, "%s\t}\n", tab_str);

    fprintf_s(out_stream, "%s}\n", tab_str);

    CLEAR_RESOURCES();
    return 0;
}
