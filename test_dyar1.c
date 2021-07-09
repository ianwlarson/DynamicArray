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

    size_t const newlen = 10 * sizeof(void *);
    void *const buffer = malloc(newlen);

    void *oldbuf;
    size_t oldlen;
    status = dyar_move(da, buffer, newlen, &oldbuf, &oldlen);
    assert(status == 0);
    assert(oldbuf == NULL);
    assert(oldlen == 0);

    unsigned a = dyar_add(da, A_VAL);
    assert(a == 0);
    unsigned b = dyar_add(da, B_VAL);
    assert(b == 1);
    unsigned c = dyar_add(da, C_VAL);
    assert(c == 2);

    assert(dyar_get(da, a) == A_VAL);
    assert(dyar_get(da, b) == B_VAL);
    assert(dyar_get(da, c) == C_VAL);

    status = dyar_free(da, a);
    assert(status == 0);
    status = dyar_free(da, b);
    assert(status == 0);
    status = dyar_free(da, c);
    assert(status == 0);

    a = dyar_add(da, A_VAL);
    b = dyar_add(da, B_VAL);
    c = dyar_add(da, C_VAL);

    assert(dyar_get(da, a) == A_VAL);
    assert(dyar_get(da, b) == B_VAL);
    assert(dyar_get(da, c) == C_VAL);

    dyar_free(da, b);

    assert(dyar_get(da, a) == A_VAL);
    assert(dyar_get(da, b) == NULL);
    assert(dyar_get(da, c) == C_VAL);

    free(buffer);
    free(da);

    printf("Ending!\n");

    return 0;
}
