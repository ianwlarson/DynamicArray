#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "dyar.h"

#define A_VAL ((void *)(1 << 2))
#define B_VAL ((void *)(2 << 2))
#define C_VAL ((void *)(3 << 2))

int
main(void)
{
    int status = 0;
    printf("Starting!\n");

    da_t *da = malloc(sizeof(*da));
    *da = (da_t) {
        0,
    };

    size_t const newlen = 22 * sizeof(void *);
    void *const buffer = malloc(newlen);

    void *oldbuf;
    size_t oldlen;
    status = dyar_move(da, buffer, newlen, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == NULL);
    assert(oldlen == 0);

    for (;;) {
        if (dyar_full(da)) {
            break;
        }

        (void)dyar_add(da, A_VAL);
    }

    for (unsigned i = 0; i < 22; ++i) {
        void *ptr = dyar_get(da, i);
        assert(ptr == A_VAL);
    }

    dyar_free(da, 11);
    size_t const newerlen = 33 * sizeof(void *);
    void *const newerbuf = malloc(newerlen);

    status = dyar_move(da, newerbuf, newerlen, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == buffer);
    assert(oldlen == newlen);

    unsigned a = dyar_add(da, A_VAL);
    assert(a == 11);
    for (unsigned i = 0; i < 22; ++i) {
        void *ptr = dyar_get(da, i);
        assert(ptr == A_VAL);
    }

    free(buffer);
    free(newerbuf);
    free(da);


    printf("Ending!\n");

    return 0;
}
