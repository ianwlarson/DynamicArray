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

    size_t const newlen = 100 * sizeof(void *);
    void *const buffer = malloc(newlen);

    void *oldbuf;
    size_t oldlen;
    status = dyar_move(da, buffer, newlen, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == NULL);
    assert(oldlen == 0);

    for (int i = 0; i < 100; ++i) {

        for (;;) {
            if (dyar_full(da)) {
                break;
            }

            dyar_add(da, malloc(13));
        }

        status = dyar_valid(da);
        assert(status == 0);

        for (int j = 0; j < 30; ++j) {
            unsigned const idx = xorshift32(&rng) % 100;
            void *ptr = dyar_get(da, idx);
            if (ptr != NULL) {
                dyar_free(da, idx);
                free(ptr);
            }
        }

        status = dyar_valid(da);
        assert(status == 0);

        for (unsigned j = 0; j < 100; ++j) {
            void *ptr = dyar_get(da, j);
            if (ptr != NULL) {
                dyar_free(da, j);
                free(ptr);
            }
        }

        status = dyar_valid(da);
        assert(status == 0);
    }

    free(buffer);
    free(da);

    printf("Ending!\n");

    return 0;
}
