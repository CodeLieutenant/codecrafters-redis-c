// #include <netinet/in.h>
// #include <netinet/ip.h>

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

int main()
{
    // Disable output buffering
    setbuf(stdout, NULL);

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    printf("Logs from your program will appear here!\n");

    // Uncomment this block to pass the first stage
    //
    // int server_fd, client_addr_len;
    // struct sockaddr_in client_addr;
    //
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        printf("Socket creation failed: %s...\n", strerror(errno));
        return 1;
    }
    //
    // // Since the tester restarts your program quite often, setting REUSE_PORT
    // // ensures that we don't run into 'Address already in use' errors
    // int reuse = 1;
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
    // 	printf("SO_REUSEPORT failed: %s \n", strerror(errno));
    // 	return 1;
    // }
    //
    // struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
    // 								 .sin_port = htons(6379),
    // 								 .sin_addr = { htonl(INADDR_ANY) },
    // 								};
    //
    // if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
    // 	printf("Bind failed: %s \n", strerror(errno));
    // 	return 1;
    // }
    //
    // int connection_backlog = 5;
    // if (listen(server_fd, connection_backlog) != 0) {
    // 	printf("Listen failed: %s \n", strerror(errno));
    // 	return 1;
    // }
    //
    // printf("Waiting for a client to connect...\n");
    // client_addr_len = sizeof(client_addr);
    //
    // accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
    // printf("Client connected\n");
    //
    // close(server_fd);

    return 0;
}