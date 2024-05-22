#pragma once

#include <array.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string_t.h>

#define REDIS_SIMPLE_STRING '+'
#define REDIS_SIMPLE_ERROR '-'
#define REDIS_INTEGER ':'
#define REDIS_BULK_STRING '$'
#define REDIS_ARRAY '*'

#define REDIS_NEW_LINE '\n'
#define REDIS_CAR_RETURN '\r'

#define IS_REDIS_COMMAND(cmd, parsed) strncasecmp(parsed.value, cmd, min(sizeof(cmd) - 1, parsed.len)) == 0

typedef enum
{
    TY_REDIS_STRING,
    TY_REDIS_ERROR,
    TY_REDIS_INTEGER,
    TY_REDIS_BULK_STRING,
    TYP_REDIS_ARRAY,
} redis_token_t;

typedef struct
{
    enum
    {
        REDIS_COMMAND_PING,
        REDIS_COMMAND_ECHO,
        REDIS_COMMAND_UNKNOWN,
    } type;

    array_redis_command_arg_t *args;
} redis_command_t;

typedef struct
{
    enum
    {
        REDIS_ARG_STRING,
        REDIS_ARG_INT,
    } type;

    union {
        int64_t integer;
        string_t *string;
    };
} redis_command_arg_t;

DYN_ARRAY_DECL(redis_command_arg_t);

typedef struct redis_node
{
    redis_token_t type;

    union {
        struct
        {
            array_redis_node_t *value;
        } array;

        struct
        {
            int64_t value;
        } integer;

        struct
        {
            size_t len;
            char *value;
        } string;
    };
} redis_node_t;

DYN_ARRAY_DECL(redis_node_t);

typedef struct
{
    const char *buf;
    size_t len;
    size_t offset;

    array_redis_node_t *root;

} redis_parser_t;

typedef struct
{
    const char *buf;
    size_t len;
} redis_buffer_t;

redis_parser_t new_parser(const char *buf, size_t len)
{
    return (redis_parser_t){.buf = buf, .len = len, .offset = 0, .root = NULL};
}

redis_buffer_t read_until_clrf(redis_parser_t *parser)
{
    for (size_t i = parser->offset, len = 0; i < parser->len - 1; i++, len++)
    {
        if (parser->buf[i] == REDIS_CAR_RETURN && parser->buf[i + 1] == REDIS_NEW_LINE)
        {
            size_t start = parser->offset;
            parser->offset = i + 2;
            return (redis_buffer_t){.buf = (const char *)&parser->buf[start], .len = len};
        }
    }

    return (redis_buffer_t){0};
}

array_redis_node_t *parse_inner(redis_parser_t *parser, size_t start, size_t suggested_len)
{
    array_redis_node_t *start = array_redis_node_t_init(suggested_len);

    for (size_t i = start; i < parser->len; i++)
    {
        switch (parser->buf[i])
        {
        case REDIS_ARRAY: {
            parser->offset++;
            redis_buffer_t array_len = read_until_clrf(parser);

            char *end;
            long long len = strtoll(array_len.buf, &end, 10);
            redis_node_t arr = {
                .type = TYP_REDIS_ARRAY,
                .array.value = parse_inner(parser, i, len),
            };

            start = array_redis_node_t_append(start, arr);

            break;
        }
        case REDIS_BULK_STRING: {
            parser->offset++;
            redis_buffer_t string_len = read_until_clrf(parser);
            redis_buffer_t string_data = read_until_clrf(parser);

            printf("String Len: %.*s", (int)string_len.len, string_len.buf);
            printf("String Data: %.*s\n", (int)string_data.len, string_data.buf);
            break;
        }
        case REDIS_SIMPLE_STRING: {
            parser->offset++;
            redis_buffer_t buf = read_until_clrf(parser);

            printf("%.*s\n", (int)buf.len, buf.buf);
            break;
        }
        }
    }

    return start;
}

redis_command_t parse_command(redis_parser_t *parser)
{
    parser->root = parse_inner(parser, 0, 1);

    switch (parser->root->data[0].type)
    {
    case TY_REDIS_STRING:
        if (IS_REDIS_COMMAND("ping", parser->root->data[0].string))
        {
            return (redis_command_t){.type = REDIS_COMMAND_PING};
        }
    case TYP_REDIS_ARRAY:
        array_redis_node_t *array = parser->root->data[0].array.value;

        if (IS_REDIS_COMMAND("ping", parser->root->data[0].string))
        {
            return (redis_command_t){.type = REDIS_COMMAND_PING};
        }

        if (IS_REDIS_COMMAND("echo", array->data[0].string))
        {
            if (array->size < 2)
            {
                // TODO: Check error
            }

            array_redis_command_arg_t *args = array_redis_command_arg_init(array->size - 1);

            for (int i = 1; i < array->size; i++)
            {
                redis_command_arg_t arg = {
                    .string = copy_string(array->data[i].string.value, array->data[i].string.len),
                    .type = REDIS_ARG_STRING,
                };

                args = array_redis_command_arg_t_append(args, arg);
            }

            return (redis_command_t){.type = REDIS_COMMAND_ECHO, .args = args};
        }
    }

    return (redis_command_t){.type = REDIS_COMMAND_UNKNOWN};
}
