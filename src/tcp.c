#include <parser.h>
#include <pch.h>
#include <tcp.h>

static void on_new_connection(uv_stream_t *server, int status);
static void allocate_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
static void on_client_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
// static void on_client_write(uv_write_t *req, int status);

int start_tcp_server(uv_loop_t *loop, const char *host, int port)
{
    uv_tcp_t *server = malloc(sizeof(uv_tcp_t));
    HANDLE_UV_ERROR(uv_tcp_init(loop, server), "Server init failed");

    sockaddr_in_t addr;
    HANDLE_UV_ERROR(uv_ip4_addr(host, port, &addr), "Failed to parse address %s:%d", host, port);
    HANDLE_UV_ERROR(uv_tcp_nodelay(server, 1), "Failed to set TCP NO_DELAY");
    HANDLE_UV_ERROR(uv_tcp_keepalive(server, 1, 60), "Failed to set TCP KeepAlive");
    HANDLE_UV_ERROR(uv_tcp_simultaneous_accepts(server, 1), "Failed to set TCP Simultaneous Accepts");
    HANDLE_UV_ERROR(uv_tcp_bind(server, (const sockaddr_t *)&addr, 0), "Server bind failed");
    HANDLE_UV_ERROR(uv_listen((uv_stream_t *)server, 1000, on_new_connection), "Server listen failed");

    return 0;
error:
    free(server);
    return 1;
}

static void on_new_connection(uv_stream_t *server, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));

    HANDLE_UV_ERROR(uv_tcp_init(server->loop, client), "Client init failed");
    HANDLE_UV_ERROR(uv_accept(server, (uv_stream_t *)client), "Accept failed");

    HANDLE_UV_ERROR(uv_read_start((uv_stream_t *)client, allocate_buffer, on_client_read),
                    "Failed to start read from the client");

    return;
error:
    uv_close((uv_handle_t *)client, NULL);
    free(client);
}

static void allocate_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    UNUSED(handle);
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

static void on_client_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    UNUSED(buf);
    uv_write_t *write_req = NULL;
    uv_buf_t write_buf = {0};

    if (nread < 0)
    {
        fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        goto error;
    }

    redis_parser_t parser = new_parser(buf->base, nread);
    redis_command_t cmd = parse_command(&parser);

    switch (cmd.type)
    {
    case REDIS_COMMAND_PING:
        write_buf.base = (char *)"+PONG\r\n";
        write_buf.len = sizeof("+PONG\r\n") - 1;
        break;
    default:
        break;
    }

    write_req = malloc(sizeof(uv_write_t));

    write_buf.base = (char *)"+PONG\r\n";
    write_buf.len = sizeof("+PONG\r\n") - 1;

    HANDLE_UV_ERROR(uv_write(write_req, stream, &write_buf, 1, NULL), "Failed to write to client");

    free(buf->base);
    return;
error:
    uv_close((uv_handle_t *)stream, NULL);
    free(buf->base);
    if (write_req != NULL)
    {
        free(write_req);
    }
}

// static void on_client_write(uv_write_t *req, int status)
// {
//     UNUSED(status);
//     for (unsigned int i = 0; i < req->nbufs; i++)
//     {
//         free(req->bufs[i].base);
//     }
// }
