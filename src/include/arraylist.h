#pragma once

#include "defines.h"

/******************************
    *
    * ARRAYLIST, SLICE, SEQUENCE
    * MACRO IMPLEMENTATION
    *
    * ************************/
#define Slice(T) Slice_##T

#define decl_slice(T)                                                                              \
    typedef struct                                                                                 \
    {                                                                                              \
        usize count;                                                                              \
        T *elements;                                                                               \
    } Slice(T)

#define seq_elem_type(seq) typeof(*(seq)->elements)
#define seq_start(seq) ((seq)->elements)
#define seq_end(seq)   ((seq)->elements + (seq)->count)
#define seq_length(seq) ((seq)->count)

#define slice_make(T, start, count) ((Slice(T)){.count = (count), .elements = (start)})
#define slice_from_array(arr, low, high)                                                           \
    ((Slice(typeof(*(arr)->elements))){                                                           \
        .count = ((high) - (low)),                                                                \
        .elements = seq_start(arr) + (low)                                                        \
    })

#define for_each(seq, iter)                                                                        \
    for (seq_elem_type(seq) *(iter) = seq_start(seq); (iter) < seq_end(seq); (iter)++)

#define Array(T) Array_##T

#define generate_array_type(T)                                                                     \
    typedef struct                                                                                 \
    {                                                                                              \
        usize count;                                                                              \
        usize capacity;                                                                           \
        T elements[];                                                                              \
    } * Array(T);                                                                                  \
    decl_slice(T)

#define array_total_size(arr)                                                                      \
    (sizeof(Array_Header) + ((arr)->capacity * sizeof(seq_elem_type(arr))))

typedef struct
{
    usize count;
    usize capacity;
} Array_Header;

static inline Array_Header *array_new(Array_Header *header, usize initial_size)
{
    if (!header)
        return NULL;
    header->count    = 0;
    header->capacity = initial_size;
    return header;
}

#define array_make(T, size)                                                                        \
    ((Array(T))array_new(malloc(sizeof(Array_Header) + (size) * sizeof(T)), (size)))

#define array_free(arr) free(arr)
#define array_push(arr, value)                                                                     \
    do                                                                                             \
    {                                                                                              \
        if ((arr)->count + 1 > (arr)->capacity)                                                    \
        {                                                                                          \
            (arr)->capacity <<= 1;                                                                 \
            (arr) = realloc((arr), array_total_size(arr));                                         \
        }                                                                                          \
        (arr)->elements[(arr)->count++] = (value);                                                 \
    } while (0)

#define array_start(arr) seq_start(arr)
#define array_end(arr)   ((arr)->elements + ((arr)->count - 1))
#define array_at(arr, idx) ((arr)->elements[(idx)])
#define array_length(arr) seq_length(arr)
#define array_for_each(arr, el) for_each(arr, el)
