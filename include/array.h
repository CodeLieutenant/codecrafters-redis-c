#pragma once

#include <pch.h>
#include <stddef.h>
#include <stdint.h>

#define DYN_ARRAY(type) array_##type

#define DYN_ARRAY_DECL(type)                                                                                           \
    typedef struct                                                                                                     \
    {                                                                                                                  \
        size_t size;                                                                                                   \
        size_t capacity;                                                                                               \
        type data[1];                                                                                                  \
    } array_##type;                                                                                                    \
    DYN_ARRAY(type) * array_##type##_init(size_t capacity)                                                             \
    {                                                                                                                  \
        DYN_ARRAY(type) *array = (DYN_ARRAY(type) *)malloc(sizeof(DYN_ARRAY(type)) + (capacity - 1) * sizeof(type));   \
        array->size = 0;                                                                                               \
        array->capacity = capacity;                                                                                    \
        return array;                                                                                                  \
    }                                                                                                                  \
    DYN_ARRAY(type) * array_##type##_append(DYN_ARRAY(type) * array, type value)                                       \
    {                                                                                                                  \
        if (array->size == array->capacity)                                                                            \
        {                                                                                                              \
            array = (DYN_ARRAY(type) *)realloc(array, sizeof(DYN_ARRAY(type)) + (array->capacity * 2) * sizeof(type)); \
            array->capacity *= 2;                                                                                      \
        }                                                                                                              \
        array->data[array->size] = value;                                                                              \
        array->size += 1;                                                                                              \
        return array;                                                                                                  \
    }                                                                                                                  \
    void array_##type##_free(DYN_ARRAY(type) * array)                                                                  \
    {                                                                                                                  \
        free(array);                                                                                                   \
    }
