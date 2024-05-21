#include <pch.h>
#include <tcp.h>

#include <signal.h>

#define REDIS_DEFAULT_HOST "0.0.0.0"
#define REDIS_DEFAULT_PORT 6379

static void on_signal_handler(uv_signal_t *handle, int signum);
static void on_uv_walk(uv_handle_t *handle, void *arg);

static void cleanup(uv_loop_t *loop)
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

    if (start_tcp_server(loop, REDIS_DEFAULT_HOST, REDIS_DEFAULT_PORT) != 0)
    {
        fprintf(stderr, "Failed to start TCP Server");
        goto error;
    }

    HANDLE_UV_ERROR(uv_run(loop, UV_RUN_DEFAULT), "Loop run failed");

    cleanup(loop);
    free(loop);
    return 0;

error:
    cleanup(loop);
    free(loop);
    return 1;
}

static void on_uv_walk(uv_handle_t *handle, void *arg)
{
    UNUSED(arg);
    uv_close(handle, NULL);

    switch (handle->type)
    {
    case UV_TCP:
        free((uv_tcp_t *)handle);
        break;
    default:
    }
}

static void on_signal_handler(uv_signal_t *handle, int signum)
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
