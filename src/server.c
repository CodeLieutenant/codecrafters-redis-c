#include <pch.h>

#include <signal.h>

typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

void on_signal_handler(uv_signal_t *handle, int signum);
void on_uv_close(uv_handle_t *handle);
void on_uv_walk(uv_handle_t *handle, void *arg);

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

    uv_timer_t timer_req;
    if (uv_timer_init(loop, &timer_req) != 0)
    {
        printf("Timer init failed: %s \n", strerror(errno));
        free(loop);
        return 1;
    }

    if (uv_timer_start(&timer_req, print_on_timer, 1000, 1000) != 0)
    {
        printf("Timer start failed: %s \n", strerror(errno));
        free(loop);
        return 1;
    }

    if (uv_run(loop, UV_RUN_DEFAULT) != 0)
    {
        printf("Loop run failed: %s \n", strerror(errno));
        free(loop);
        return 1;
    }

    cleanup(loop);
    free(loop);
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
