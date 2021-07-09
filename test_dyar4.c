#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "dyar.h"

static inline unsigned
xorshift32(unsigned *const rng_state)
{
    unsigned x = *rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *rng_state = x;
    return *rng_state;
}

int
main(void)
{
    int status = 0;
    printf("Starting!\n");

    da_t *da = malloc(sizeof(*da));
    *da = (da_t) {
        0,
    };

    size_t const newlen = 256 * sizeof(void *);
    void *const buffer = malloc(newlen);

    void *oldbuf;
    size_t oldlen;
    status = dyar_move(da, buffer, newlen, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == NULL);
    assert(oldlen == 0);

    unsigned rng = time(NULL);
    for (int i = 0; i < (getpid() & 0xfff); ++i) {
        (void)xorshift32(&rng);
    }

    for (unsigned i = 0; i < 256; ++i) {
        (void)dyar_add(da, malloc(123));
    }

    for (int i = 0; i < 1000000; ++i) {

        unsigned idx = xorshift32(&rng) & 0xffu;
        if (dyar_idx_free(da, idx)) {
            (void)dyar_add(da, malloc(123));
        } else {
            void *ptr = dyar_get(da, idx);

            // Write into the pointers, fsanitize=address will catch us
            // following bad ptrs.
            memset(ptr, 1, 123);
            status = dyar_free(da, idx);
            assert(status == 0);
            free(ptr);
        }
    }

    // Free all the entries. If we miss any the address sanitizer will let us
    // know.
    for (unsigned i = 0; i < 256; ++i) {
        void *ptr = dyar_get(da, i);
        if (ptr != NULL) {
            memset(ptr, 2, 123);
            dyar_free(da, i);
            free(ptr);
        }
    }

    free(buffer);
    free(da);

    printf("Ending!\n");

    return 0;
}
