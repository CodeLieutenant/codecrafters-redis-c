#include <pch.h>

#include <signal.h>

#define REDIS_DEFAULT_HOST "0.0.0.0"
#define REDIS_DEFAULT_PORT 6379

typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

void on_signal_handler(uv_signal_t *handle, int signum);
void on_uv_close(uv_handle_t *handle);
void on_uv_walk(uv_handle_t *handle, void *arg);
void on_new_connection(uv_stream_t *server, int status);

void print_on_timer(uv_timer_t *handle)
{
    UNUSED(handle);
    printf("Hello world\n");
}

void cleanup(uv_loop_t *loop)
{
    if (uv_loop_close(loop) == UV_EBUSY)
    {
        uv_stop(loop);
        uv_walk(loop, on_uv_walk, NULL);
    }
}

int main(void)
{
    setbuf(stdout, NULL);

    pid_t current_pid = getpid();

    printf("Current PID: %d, MiMalloc Version: %d\n", current_pid, mi_version());

    uv_loop_t *loop = malloc(sizeof(uv_loop_t));

    if (uv_loop_init(loop) != 0)
    {
        printf("Loop init failed: %s \n", strerror(errno));
        free(loop);
        return 1;
    }

    uv_signal_t sigterm, sigint;

    uv_signal_init(loop, &sigterm);
    uv_signal_init(loop, &sigint);
    uv_signal_start(&sigterm, on_signal_handler, SIGTERM);
    uv_signal_start(&sigint, on_signal_handler, SIGINT);

    // uv_timer_t timer_req;
    // if (uv_timer_init(loop, &timer_req) != 0)
    // {
    //     printf("Timer init failed: %s \n", strerror(errno));
    //     free(loop);
    //     return 1;
    // }

    // if (uv_timer_start(&timer_req, print_on_timer, 1000, 1000) != 0)
    // {
    //     printf("Timer start failed: %s \n", strerror(errno));
    //     goto cleanup;
    // }

    uv_tcp_t server;
    if (uv_tcp_init(loop, &server) != 0)
    {
        printf("Server init failed: %s \n", uv_err_name(errno));
        goto cleanup;
    }

    sockaddr_in_t addr;
    if (uv_ip4_addr(REDIS_DEFAULT_HOST, REDIS_DEFAULT_PORT, &addr) != 0)
    {
        printf("Failed to parse address %s:%d\n", REDIS_DEFAULT_HOST, REDIS_DEFAULT_PORT);
        goto cleanup;
    }

    uv_tcp_nodelay(&server, 1);
    uv_tcp_keepalive(&server, 1, 60);
    uv_tcp_simultaneous_accepts(&server, 1);

    if (uv_tcp_bind(&server, (const sockaddr_t *)&addr, 0) != 0)
    {
        printf("Server bind failed: %s \n", uv_err_name(errno));
        goto cleanup;
    }

    if (uv_listen((uv_stream_t *)&server, 1000, on_new_connection) != 0)
    {
        printf("Server listen failed: %s \n", uv_err_name(errno));
        goto cleanup;
    }

    if (uv_run(loop, UV_RUN_DEFAULT) != 0)
    {
        printf("Loop run failed: %s \n", uv_err_name(errno));
        goto cleanup;
    }

    cleanup(loop);
    free(loop);
    return 0;

cleanup:
    cleanup(loop);
    free(loop);
    return 1;
}

void on_uv_close(uv_handle_t *handle)
{
    switch (handle->type)
    {
    case UV_TIMER:
        uv_timer_stop((uv_timer_t *)handle);
        printf("Timer closed\n");
        break;
    case UV_SIGNAL:
        uv_signal_stop((uv_signal_t *)handle);
        printf("Signal closed\n");
        break;
    case UV_TCP:
        uv_close(handle, NULL);
        printf("TCP closed\n");
        break;
    default:
        printf("Unknown handle type: %d\n", handle->type);
        break;
    }
}

void on_uv_walk(uv_handle_t *handle, void *arg)
{
    UNUSED(arg);
    uv_close(handle, on_uv_close);
}

void on_signal_handler(uv_signal_t *handle, int signum)
{
    UNUSED(signum);
    if (uv_signal_stop(handle) != 0)
    {
        printf("Signal stop failed: %s \n", strerror(errno));
        exit(1);
    }

    cleanup(handle->loop);

    printf("Exiting....\n");
}

void on_new_connection(uv_stream_t *server, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));

    uv_tcp_init(server->loop, client);

    if (uv_accept(server, (uv_stream_t *)client) == 0)
    {
        fprintf(stdout, "New connection\n");
        // uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
    }
}
