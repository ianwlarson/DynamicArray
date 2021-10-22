
#include "dyar_check.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

int
dyar_valid(da_t const*const p_da)
{
    int status = 0;
    bool *reachable = malloc(p_da->m_capacity * sizeof(*reachable));

    for (int i = 0; i < p_da->m_capacity; ++i) {
        reachable[i] = false;
    }

    // First, follow the free list and mark all nodes that can be found via the
    // free list as reachable.
    unsigned last = 0;
    unsigned cursor = p_da->m_free;
    while (cursor != 0) {
        unsigned const idx = cursor - 1;
        if (idx >= p_da->m_capacity) {
            printf("Found an idx on the free list beyond the capacity!\n");
            status = -1;
            goto done;
        }

        if (idx >= p_da->m_allocated) {
            printf("Found an idx (%u -> %u) on the free list beyond the allocated entries!\n", last, idx);
            status = -1;
            goto done;
        }

        reachable[idx] = true;

        uintptr_t const v = p_da->m_data[idx];
        if ((v & DYAR_FREE) != DYAR_FREE) {
            printf("Found an entry on the free list without DYAR_FREE set!\n");
            status = -1;
            goto done;
        }
        uintptr_t const nxt = v>>2;
        if (nxt > DYAR_MAX) {
            printf("Found an entry on the free list pointing to > DYAR_MAX!\n");
            status = -1;
            goto done;
        }
        cursor = (unsigned)nxt;
        last = idx;
    }

    // Next, iterate over all allocated entries of the array and make sure that
    // any free entries are reachable via the free list.
    for (unsigned i = 0; i < p_da->m_allocated; ++i) {
        uintptr_t const v = p_da->m_data[i];
        if ((v & DYAR_FREE) == DYAR_FREE) {

            if (!reachable[i]) {
                printf("Found a free element that isn't on the free list!\n");
                status = -1;
                goto done;
            }

        }
    }

done:
    free(reachable);
    return status;
}
