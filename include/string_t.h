#pragma once

#include <pch.h>

typedef struct
{
    size_t len;
    char data[1];
} string_t;

string_t *new_string(size_t len)
{
    string_t *str = (string_t *)malloc(sizeof(string_t) + len);
}

void free_string(string_t *str)
{
    free(str);
}

string_t *copy_string(const char *str, size_t len)
{
    string_t *copy = new_string(len);
    memcpy(copy->data, str, len);
    copy->len = len;
    return copy;
}