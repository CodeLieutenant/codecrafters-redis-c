#include <pch.h>

#include <signal.h>

#define REDIS_DEFAULT_HOST "0.0.0.0"
#define REDIS_DEFAULT_PORT 6379

#define HANDLE_UV_ERROR(status, message, ...)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        int __status = (status);                                                                                       \
        if ((__status) < 0)                                                                                            \
        {                                                                                                              \
            fprintf(stderr, message "  UV error: %s (%s)\n", ##__VA_ARGS__, uv_err_name((__status)),                   \
                    uv_strerror((__status)));                                                                          \
            goto error;                                                                                                \
        }                                                                                                              \
    } while (0)

typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

void on_signal_handler(uv_signal_t *handle, int signum);
void on_uv_close(uv_handle_t *handle);
void on_uv_walk(uv_handle_t *handle, void *arg);
void on_new_connection(uv_stream_t *server, int status);

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

    HANDLE_UV_ERROR(uv_loop_init(loop), "Loop init failed");

    uv_signal_t sigterm, sigint;
    HANDLE_UV_ERROR(uv_signal_init(loop, &sigterm), "Signal init failed SIGNAL(%d)", SIGTERM);
    HANDLE_UV_ERROR(uv_signal_init(loop, &sigint), "Signal init failed SIGNAL(%d)", SIGINT);
    HANDLE_UV_ERROR(uv_signal_start(&sigterm, on_signal_handler, SIGTERM), "Signal start failed SIGNAL(%d)", SIGTERM);
    HANDLE_UV_ERROR(uv_signal_start(&sigint, on_signal_handler, SIGINT), "Signal start failed SIGNAL(%d)", SIGINT);

    uv_tcp_t server;
    HANDLE_UV_ERROR(uv_tcp_init(loop, &server), "Server init failed");

    sockaddr_in_t addr;
    HANDLE_UV_ERROR(uv_ip4_addr(REDIS_DEFAULT_HOST, REDIS_DEFAULT_PORT, &addr), "Failed to parse address %s:%d",
                    REDIS_DEFAULT_HOST, REDIS_DEFAULT_PORT);
    HANDLE_UV_ERROR(uv_tcp_nodelay(&server, 1), "Failed to set TCP NO_DELAY");
    HANDLE_UV_ERROR(uv_tcp_keepalive(&server, 1, 60), "Failed to set TCP KeepAlive");
    HANDLE_UV_ERROR(uv_tcp_simultaneous_accepts(&server, 1), "Failed to set TCP Simultaneous Accepts");

    HANDLE_UV_ERROR(uv_tcp_bind(&server, (const sockaddr_t *)&addr, 0), "Server bind failed");
    HANDLE_UV_ERROR(uv_listen((uv_stream_t *)&server, 1000, on_new_connection), "Server listen failed");
    HANDLE_UV_ERROR(uv_run(loop, UV_RUN_DEFAULT), "Loop run failed");

    cleanup(loop);
    free(loop);
    return 0;

error:
    cleanup(loop);
    free(loop);
    return 1;
}

void on_uv_close(uv_handle_t *handle)
{
    switch (handle->type)
    {
    case UV_TIMER:
        HANDLE_UV_ERROR(uv_timer_stop((uv_timer_t *)handle), "Timer stop failed");
        printf("Timer closed\n");
        break;
    case UV_SIGNAL:
        HANDLE_UV_ERROR(uv_signal_stop((uv_signal_t *)handle), "Signal stop failed");
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
error:
}

void on_uv_walk(uv_handle_t *handle, void *arg)
{
    UNUSED(arg);
    uv_close(handle, on_uv_close);
}

void on_signal_handler(uv_signal_t *handle, int signum)
{
    UNUSED(signum);
    HANDLE_UV_ERROR(uv_signal_stop(handle), "Loop stop failed");

    printf("Exiting....\n");
    cleanup(handle->loop);
    return;
error:
    cleanup(handle->loop);
    exit(1);
}

void on_new_connection(uv_stream_t *server, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));

    HANDLE_UV_ERROR(uv_tcp_init(server->loop, client), "Client init failed");
    HANDLE_UV_ERROR(uv_accept(server, (uv_stream_t *)client), "Accept failed");

    printf("New connection\n");
    // uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);

error:
    uv_tcp_close_reset(client, NULL);
    uv_close((uv_handle_t *)client, NULL);
    free(client);
}
