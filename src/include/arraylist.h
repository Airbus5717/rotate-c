#pragma once

#include "defines.h"

// General sequence macros
#define seq_elem_type(seq) typeof(*(seq)->elements)

#define seq_start(seq) ((seq)->elements)
#define seq_end(seq)   ((seq)->elements + (seq)->count)

#define seq_len(seq) ((seq)->count)

#define make_slice(T, start, count) ((Slice(T)){start, count})
#define arr_slice(arr, low, high)                                                                  \
    {                                                                                              \
        (high) - (low), arr_start(arr) + (low)                                                     \
    }

#define for_each(seq, iter)                                                                        \
    for (seq_elem_type(seq) *iter = seq_start(seq); iter < seq_end(seq); iter++)

// Slice type

#define Slice(T) Slice_##T

#define decl_slice(T)                                                                              \
    typedef struct                                                                                 \
    {                                                                                              \
        size_t count;                                                                              \
        T *elements;                                                                               \
    } Slice(T)

// arraylist type

// #define ARRAY_GROWTH_FACTOR 2
#define ArrayList(T) ArrayList_##T
#define generate_array_type(T)                                                                     \
    typedef struct T                                                                               \
    {                                                                                              \
        usize count;                                                                               \
        usize capacity;                                                                            \
        T elements[];                                                                              \
    } * ArrayList(T);                                                                              \
    decl_slice(T)

// arraylist implementation

#define array_total_size(arr)                                                                      \
    (sizeof(_arraylist_header) + (arr)->capacity * sizeof(seq_elem_type(arr)))

typedef struct
{
    usize count;
    usize capacity;
} _arraylist_header;

static inline _arraylist_header *
_new_arr(_arraylist_header *header, usize initial_size)
{
    header->count    = 0;
    header->capacity = initial_size;
    return header;
}

#define make_array(T, size)                                                                        \
    ((ArrayList(T))_new_arr(mem_alloc(sizeof(_arraylist_header) + size * sizeof(T)), size))
#define array_free(arr) mem_free(arr)

#define array_push(arr, ...)                                                                       \
    do                                                                                             \
    {                                                                                              \
        if ((arr)->count + 1 > (arr)->capacity)                                                    \
        {                                                                                          \
            (arr)->capacity <<= 1;                                                                 \
            (arr) = mem_resize(arr, array_total_size(arr));                                        \
        }                                                                                          \
        (arr)->elements[(arr)->count++] = __VA_ARGS__;                                             \
    } while (0);

#define array_start(arr) seq_start(arr)
#define array_end(arr)   (array_at(arr, (array_len(arr) - 1)))
// WARN: no bounds checking
#define array_at(arr, idx) (arr)->elements[(idx)]

#define array_len(arr) seq_len(arr)

#define array_foreach(arr, el) for_each(arr, el)

// end of arr.H
