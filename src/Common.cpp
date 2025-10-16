#include "Common.h"

#undef FINAL_CODE
#define FINAL_CODE

errno_t My_calloc(void **const dest, size_t const num, size_t const size) {
    assert(dest);

    if (num == 0) { *dest = nullptr; return 0; }

    void *ptr = calloc(num, size);
    if (!ptr) { return errno; }

    *dest = ptr;
    return 0;
}
