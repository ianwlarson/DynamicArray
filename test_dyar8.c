#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

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

    size_t const newlen = 128 * sizeof(void *);
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

    for (;;) {

        if (dyar_full(da)) {
            break;
        }

        (void)dyar_add(da, malloc(13));
    }

    status = dyar_valid(da);
    assert(status == 0);

    // Free all the odd numbered entries
    for (unsigned i = 1; i < da->m_allocated; i+=2) {
        void *ptr = dyar_get(da, i);
        assert(ptr != NULL);
        free(ptr);
        status = dyar_free(da, i);
        assert(status == 0);
    }

    status = dyar_valid(da);
    assert(status == 0);

    assert(dyar_size(da) == 64);

    size_t const resizelen = 129 * sizeof(void *);
    void *resize_buf = malloc(resizelen);

    status = dyar_move(da, resize_buf, resizelen, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == buffer);
    assert(oldlen == newlen);

    status = dyar_valid(da);
    assert(status == 0);

    // Clear the old buffer explicitly.
    for (int i = 0; i < 128; ++i) {
        ((uintptr_t *)buffer)[i] = 0;
    }

    status = dyar_valid(da);
    assert(status == 0);

    int added = 0;
    for (;;) {
        if (dyar_full(da)) {
            break;
        }

        dyar_add(da, malloc(13));
        ++added;
    }

    assert(added == 65);

    for (unsigned i = 0; i < da->m_allocated; ++i) {
        void *ptr = dyar_get(da, i);
        if (ptr != NULL) {
            free(ptr);
            status = dyar_free(da, i);
            assert(status == 0);
        }
    }

    status = dyar_valid(da);
    assert(status == 0);

    free(resize_buf);
    free(buffer);
    free(da);

    printf("Ending!\n");

    return 0;
}
