#pragma once

#include <uv.h>

typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

int start_tcp_server(uv_loop_t *loop, const char *host, int port);
