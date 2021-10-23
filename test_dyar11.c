#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#include "dyar_check.h"

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

    unsigned rng = time(NULL);
    for (int i = 0; i < (getpid() & 0xfff); ++i) {
        (void)xorshift32(&rng);
    }

    size_t const lenA = 100 * sizeof(void *);
    void *const bufA = malloc(lenA);
    size_t const lenB = 50 * sizeof(void *);
    void *const bufB = malloc(lenB);

    void *oldbuf;
    size_t oldlen;
    status = dyar_move(da, bufA, lenA, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == NULL);
    assert(oldlen == 0);

    for (int i = 0; i < 30; ++i) {
        unsigned v;
        status = dyar_add_safe(da, malloc(13), &v);
        assert(status == 0);
    }

    status = dyar_move(da, bufB, lenB, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == bufA);
    assert(oldlen == lenA);


    status = dyar_valid(da);
    assert(status == 0);

    // Clear the old buffer explicitly.
    for (int i = 0; i < 100; ++i) {
        ((uintptr_t *)bufA)[i] = 0;
    }

    status = dyar_valid(da);
    assert(status == 0);

    for (unsigned i = 0; i < da->m_allocated; ++i) {
        void *ptr = dyar_get(da, i);
        if (ptr != NULL) {
            free(ptr);
            dyar_free(da, i);
        }
    }

    status = dyar_valid(da);
    assert(status == 0);

    free(bufA);
    free(bufB);
    free(da);

    printf("Ending!\n");

    return 0;
}
