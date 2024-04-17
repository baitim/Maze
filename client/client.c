#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uv.h>
#include <curses.h>
#include <math.h>

uv_loop_t* loop;
uv_udp_t client_handle;

#define MAX_PLAYERS 5
#define MAX_SIZE_BUFFER 1000

void on_send(uv_udp_send_t* req, int status)
{
	free(req);
}

void alloc_buffer(uv_handle_t* client, size_t sz, uv_buf_t* buf)
{
	buf->base = malloc(sz);
	buf->len = sz;
}

void send_control()
{
	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));

	char* buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
	memcpy(buffer, "msg ", 4);

	char input_buf[MAX_SIZE_BUFFER] = "";
	scanf("%s", input_buf);

	buffer = strcat(buffer, input_buf);

	size_t sz = strlen(buffer);

	uv_buf_t buf = uv_buf_init(buffer, sz);
	uv_udp_send(req, &client_handle, &buf, 1, NULL, on_send);
}

void on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
		const struct sockaddr* addr, unsigned flags)
{
	fprintf(stderr, "%s\n", buf->base);

	send_control();
}

void rally_connect()
{
	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));

	char* buf = strdup("connect");
	size_t sz = strlen(buf);

	uv_buf_t buffer = uv_buf_init(buf, sz);

	uv_udp_send(req, &client_handle, &buffer, 1, NULL, on_send);
}

int main(int argc, char* argv[])
{
	struct sockaddr_in addr;
	char* servername = "192.168.3.10";
	
	loop = uv_default_loop();
	uv_udp_init(loop, &client_handle);
	uv_ip4_addr(servername, 9973, &addr);

	uv_udp_connect(&client_handle,  (const struct sockaddr*)&addr);
	uv_udp_recv_start(&client_handle, alloc_buffer, on_recv);
	rally_connect();
	int result = uv_run(loop, UV_RUN_DEFAULT);

	return result;

}