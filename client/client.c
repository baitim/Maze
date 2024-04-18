#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uv.h>
#include <curses.h>
#include <math.h>

uv_loop_t* loop;
uv_udp_t client_handle;

int id = -1;

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

void send_text(int i, char* buf)
{
	size_t sz = strlen(buf);

	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));
	uv_buf_t wbuf = uv_buf_init(buf, sz);
	uv_req_set_data((uv_req_t*)req, buf);
	uv_udp_send(req, &client_handle, &wbuf, 1, NULL, on_send);
}

void set_id(char* message)
{
	if (strncmp(message, "id ", 3) == 0)
		id = atoi(message + 3);
}

void on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
		const struct sockaddr* addr, unsigned flags)
{
	fprintf(stderr, "recv: %s\n", buf->base);

	if (id == -1)
		set_id(buf->base);
}

void rally_connect()
{
	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));

	char* buf = strdup("connect");
	size_t sz = strlen(buf);

	uv_buf_t buffer = uv_buf_init(buf, sz);

	uv_udp_send(req, &client_handle, &buffer, 1, NULL, on_send);
}

void on_timer(uv_timer_t* timer)
{
	if (id != -1) {
		char* buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
		sprintf(buffer, "msg %d", id);
		send_text(id, buffer);
	}
}

int main(int argc, char* argv[])
{
	uv_timer_t timer;
	struct sockaddr_in addr;
	
	loop = uv_default_loop();
	uv_udp_init(loop, &client_handle);
	uv_ip4_addr("89.223.71.143", 5002, &addr);

	uv_udp_connect(&client_handle,  (const struct sockaddr*)&addr);
	uv_udp_recv_start(&client_handle, alloc_buffer, on_recv);
	rally_connect();

	uv_timer_init(loop, &timer);
	printf("Starting timer\n");
	uv_timer_start(&timer, on_timer, 25, 25);

	int result = uv_run(loop, UV_RUN_DEFAULT);
	return result;

}