#pragma once

#include <config.h>

#ifdef HAVE_STDIO_H
#include <stdio.h>
#else
#error "Missing stdio.h"
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error "Missing stdlib.h"
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error "Missing string.h"
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#else
#error "Missing sys/socket.h"
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error "Missing errno.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error "Missing unistd.h"
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#else
#error "Missing netinet/in.h"
#endif

#ifdef HAVE_NETINET_IP_H
#include <netinet/ip.h>
#else
#error "Missing netinet/ip.h"
#endif

#include <mimalloc.h>

#include <mimalloc-override.h>

#include <uv.h>

#define UNUSED(x) (void)(x)