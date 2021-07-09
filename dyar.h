#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#define DYAR_FREE ((uintptr_t)0x1)
#define DYAR_FLAG ((uintptr_t)0x2)
#define DYAR_MASK ((uintptr_t)0x3)
#define DYAR_MAX  (UINT_MAX >> 2)

typedef struct dynamic_array_type da_t;

struct dynamic_array_type {
    uintptr_t  *m_data;
    unsigned    m_free;
    unsigned    m_capacity;
    unsigned    m_allocated;
    unsigned    m_size;
};

__attribute__((const))
static inline void *
dptr(uintptr_t const p) { return (void *)(p & ~DYAR_MASK); }

__attribute__((pure))
static inline bool
dyar_idx_free(da_t const*const p_da, unsigned const p_idx)
{
    if (p_idx >= p_da->m_allocated) return true;
    return (p_da->m_data[p_idx] & DYAR_FREE) == DYAR_FREE;
}

__attribute__((pure))
static inline unsigned
dyar_size(da_t const*const p_da)
{
    return p_da->m_size;
}

__attribute__((pure))
static inline int
dyar_cap(da_t const*const p_da)
{
    return p_da->m_capacity;
}

__attribute__((pure))
static inline bool
dyar_full(da_t const*const p_da)
{
    return p_da->m_size == p_da->m_capacity;
}

static inline unsigned
dyar_add(da_t *const p_da, void *const p_ptr)
{
    uintptr_t *spot;
    unsigned idx;

    if (p_da->m_free != 0) {

        idx = p_da->m_free - 1;
        spot = &p_da->m_data[idx];

        // Adjust the free list
        unsigned const next_spot = (*spot >> 2);
        p_da->m_free = next_spot;
    } else {
        idx = p_da->m_allocated++;
        spot = &p_da->m_data[idx];
    }
    *spot = (uintptr_t)p_ptr;
    ++p_da->m_size;

    return idx;
}

static inline int
dyar_add_safe(da_t *const p_da, void *const p_ptr, unsigned *const r_idx)
{
    if (dyar_full(p_da)) return -1;

    *r_idx = dyar_add(p_da, p_ptr);
    return 0;
}

static inline int
dyar_free(da_t *const p_da, unsigned const p_idx)
{
    if ((p_idx >= p_da->m_allocated) || dyar_idx_free(p_da, p_idx)) {
        return -1;
    }

    if (p_idx == (p_da->m_allocated - 1)) {
        // If the index we're freeing is the last allocated one, we can skip
        // the freelist and just decrement m_allocated.
        --p_da->m_allocated;
    } else {
        uintptr_t *spot = &p_da->m_data[p_idx];
        *spot = (uintptr_t)(p_da->m_free << 2) | DYAR_FREE;
        p_da->m_free = p_idx + 1;
    }

    --p_da->m_size;

    if (p_da->m_size == 0) {
        // If the array has become empty, reset it.
        p_da->m_allocated = 0;
        p_da->m_free = 0;
    }

    return 0;
}

__attribute__((pure))
static inline void *
dyar_get(da_t *const p_da, unsigned const p_idx)
{
    if (p_idx >= p_da->m_allocated) {
        return NULL;
    }

    uintptr_t const v = p_da->m_data[p_idx];
    if (v & DYAR_FREE) {
        return NULL;
    }

    return dptr(v);
}

static inline int
dyar_move(
    da_t *const p_da,
    void *const p_newbuff,
    size_t const p_bufflen,
    void **const r_oldbuf,
    size_t *const r_oldlen
)
{
    uintptr_t *const nbuff = p_newbuff;
    size_t const newcap = p_bufflen / sizeof(*nbuff);
    if (newcap > DYAR_MAX) {
        // If cap == DYAR_MAX, the highest available index will be DYAR_MAX-1,
        // and if it was on the free-list, the entry pointing to it would have
        // DYAR_MAX, which is fine.
        return -1;
    }
    if ((p_bufflen & (sizeof(*nbuff)-1)) != 0) {
        // The buffer length isn't evenly divided by a pointer.
        return -1;
    }

    if (newcap >= p_da->m_capacity) {
        // Increasing the size of the dynamic array
        for (unsigned i = 0; i < p_da->m_allocated; ++i) {
            nbuff[i] = p_da->m_data[i];
        }

        *r_oldbuf = p_da->m_data;
        *r_oldlen = p_da->m_capacity * sizeof(*nbuff);

        p_da->m_data = nbuff;
        p_da->m_capacity = newcap;

    } else {
        // Decreasing the size of the dynamic array
        if (newcap >= p_da->m_allocated) {

            // The capacity is decreasing but the indexes in the difference
            // haven't been touched.
            for (unsigned i = 0; i < p_da->m_allocated; ++i) {
                nbuff[i] = p_da->m_data[i];
            }

            *r_oldbuf = p_da->m_data;
            *r_oldlen = p_da->m_capacity * sizeof(*nbuff);

            p_da->m_data = nbuff;
            p_da->m_capacity = newcap;
        } else {

            // This is the annoying case. We're decreasing the size of the
            // array but some of the elements we're removing are on the free
            // list.
            // TODO: Implement this
            return -1;

            for (unsigned i = newcap; i < p_da->m_allocated; ++i) {
                if ((p_da->m_data[i] & DYAR_FREE) != DYAR_FREE) {
                    return -1;
                }
            }
        }
    }

    return 0;
}
