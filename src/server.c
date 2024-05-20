#include <netinet/in.h>
#include <netinet/ip.h>

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

typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

int main()
{
    setbuf(stdout, NULL);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        printf("Socket creation failed: %s...\n", strerror(errno));
        return 1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
    {
        printf("SO_REUSEPORT failed: %s \n", strerror(errno));
        return 1;
    }

    sockaddr_in_t serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6379),
        .sin_addr = {htonl(INADDR_ANY)},
    };

    if (bind(server_fd, (sockaddr_t *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        printf("Bind failed: %s \n", strerror(errno));
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0)
    {
        printf("Listen failed: %s \n", strerror(errno));
        return 1;
    }

    printf("Waiting for a client to connect...\n");
    sockaddr_in_t client_addr;
    size_t client_addr_len = sizeof(client_addr);

    accept(server_fd, (sockaddr_t *)&client_addr, (socklen_t *)&client_addr_len);
    printf("Client connected\n");

    close(server_fd);

    return 0;
}
