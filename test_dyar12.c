#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#include "dyar_check.h"

#define FOURTY_EIGHT_PERCENT 2061584301u

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

static void
remove_random_entry(da_t *const p_da, unsigned *const p_rng)
{
    if (dyar_size(p_da) == 0) {
        return;
    }

    for (;;) {
        unsigned const y = xorshift32(p_rng) % p_da->m_capacity;
        void *ptr = dyar_free_f(p_da, y);
        if (ptr != (void *)-1) {
            free(ptr);
            break;
        }
    }
}



int
main(void)
{
    int status = 0;
    printf("Starting!\n");

    int num_resizes = 10;
    da_t *da = malloc(sizeof(*da));
    *da = (da_t) {
        0,
    };

    unsigned rng = time(NULL);
    for (int i = 0; i < (getpid() & 0xfff); ++i) {
        (void)xorshift32(&rng);
    }

    unsigned cap = 64;
    size_t lenA = cap * sizeof(void *);
    void *bufA = malloc(lenA);

    void *oldbuf;
    size_t oldlen;
    status = dyar_move(da, bufA, lenA, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == NULL);
    assert(oldlen == 0);

    unsigned c = 0;

    for (;;) {

        unsigned const x = xorshift32(&rng);

        if (x < FOURTY_EIGHT_PERCENT) {
            // Make removing from the array slightly less likely than adding to
            // it.
            remove_random_entry(da, &rng);

        } else {
            void *ptr = malloc(13);
            assert(ptr != NULL);
            unsigned idx;
            status = dyar_add_safe(da, ptr, &idx);
            if (status == -1) {
                // The array ran out of space, let's resize it!
                if (num_resizes-- <= 0) {
                    free(ptr);
                    // We've already resized 10 times, lets bail.
                    break;
                }

                // Remove half the entries in a random way so the move has to
                // take into consideration the free list.
                unsigned const half_cap = cap >> 1;
                while (dyar_size(da) > half_cap) {
                    remove_random_entry(da, &rng);
                }

                // approximate multiplying by 2.5
                cap *= 5;
                cap >>= 1;
                printf("Resizing array to capacity %u\n", cap);
                size_t const nsize = cap * sizeof(void *);
                void *const nbuf = malloc(nsize);
                status = dyar_move(da, nbuf, nsize, &oldbuf, &oldlen);
                assert(status == 0);
                free(oldbuf);

                status = dyar_add_safe(da, ptr, &idx);
                assert(status == 0);
            }
        }

        if ((c++ & 0xfff) == 0) {
            status = dyar_valid(da);
            assert(status == 0);
        }
    }

    status = dyar_valid(da);
    assert(status == 0);

    for (unsigned i = 0; i < da->m_allocated; ++i) {
        void *ptr = dyar_free_f(da, i);
        if (ptr != (void *)-1) {
            free(ptr);
        }
    }

    status = dyar_valid(da);
    assert(status == 0);

    free(da->m_data);
    free(da);

    printf("Ending!\n");

    return 0;
}
