#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "dyar.h"
#include "dyar_check.h"

#define A_VAL ((void *)(1 << 2))
#define B_VAL ((void *)(2 << 2))
#define C_VAL ((void *)(3 << 2))

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

    for (int i = 0; i < 1000000; ++i) {

        unsigned idx = xorshift32(&rng) & 0xffu;
        if (dyar_idx_free(da, idx)) {
            (void)dyar_add(da, A_VAL);
        } else {
            dyar_free(da, idx);
        }

        if ((i & 0xfff) == 0) {
            status = dyar_valid(da);
            assert(status == 0);
        }
    }

    printf("size is %u\n", da->m_size);

    free(buffer);
    free(da);


    printf("Ending!\n");

    return 0;
}
