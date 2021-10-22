#pragma once

#include <stddef.h>
#include <stdint.h>

//
// This header defines a dynamic array data structure allocates indexes in the
// array while maintaining a list of free indexes. It also supports moving the
// array to different buffers.
//
// m_free = 0x2 --+
//                |
// m_data         v
//    [ INUSE ][ FREE ][ INUSE ][ FREE ]
//                v                ^
//             0x4 << 2            |
//                 +----------------
//
// m_free has a 1-based index of a free index in the array
// Each free index has a 1-based index left shifted by 2 of another free index,
// or 0 which indicates no next index.
//

#define DYAR_FREE ((uintptr_t)0x1)
#define DYAR_FLAG ((uintptr_t)0x2)
#define DYAR_MASK ((uintptr_t)0x3)
#define DYAR_MAX  ((unsigned)((~0U) >> 2))

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
dptr(uintptr_t const p) {
    return (void *)(p & ~DYAR_MASK);
}

__attribute__((pure))
static inline _Bool
dyar_idx_free(da_t const*const p_da, unsigned const p_idx)
{
    if (p_idx >= p_da->m_allocated) return 1;
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
static inline _Bool
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

    *r_oldbuf = p_da->m_data;
    *r_oldlen = p_da->m_capacity * sizeof(*nbuff);

    if (newcap >= p_da->m_capacity) {
        // Increasing the size of the dynamic array is trivial, just copy the
        // data directly.
        __builtin_memcpy(nbuff, p_da->m_data, p_da->m_allocated * sizeof(uintptr_t));

        p_da->m_data = nbuff;
        p_da->m_capacity = newcap;

    } else {

        // Decreasing the size of the dynamic array
        if (newcap >= p_da->m_allocated) {

            // The capacity is decreasing but the indexes in the difference
            // haven't been touched. We can copy directly to the new array.
            __builtin_memcpy(nbuff, p_da->m_data, p_da->m_allocated * sizeof(uintptr_t));

            p_da->m_data = nbuff;
            p_da->m_capacity = newcap;
        } else {

            // See if we can decrease the size of the array. This is a tricky
            // operation because it requires recreating the free list, which
            // requires a bunch of extra checks compared to an optimized
            // memcpy.

            for (unsigned i = newcap; i < p_da->m_allocated; ++i) {
                // There were actual in-use indices greater than the new
                // capacity, the resize cannot be done.
                if ((p_da->m_data[i] & DYAR_FREE) != DYAR_FREE) {
                    return -1;
                }
            }

            // Nothing beyond newcap is actually in-use, but it could be on the
            // free list. Recreate the freelist

            unsigned last_free = 0;
            p_da->m_free = 0;
            for (unsigned i = 0; i < newcap; ++i) {

                uintptr_t const v = p_da->m_data[i];

                if ((v & DYAR_FREE) == DYAR_FREE) {
                    if (p_da->m_free == 0) {
                        // The first time we find a free index, point m_free at
                        // it, and keep track of it.
                        last_free = i;
                        p_da->m_free = i+1;
                    } else {
                        // Every subsequent free index, update the last free
                        // index to point at it.
                        nbuff[last_free] = ((i+1) << 2) | DYAR_FREE;
                        last_free = i;
                    }
                } else {
                    // Copy in-use elements directly
                    nbuff[i] = v;
                }
            }

            // Update the very last free index found to the "NULL pointer".
            nbuff[last_free] = DYAR_FREE;

            p_da->m_data = nbuff;
            p_da->m_allocated = newcap;
            p_da->m_capacity = newcap;
        }
    }

    return 0;
}
