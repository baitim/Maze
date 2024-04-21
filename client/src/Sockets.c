#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Sockets.h"
#include "Control.h"
#include "Config.h"
#include "Window.h"

#define MAX_SIZE_BUFFER 10000

uv_udp_t client_handle;
int player_id = -1;
ErrorCode sockets_error = ERROR_NO;

MapInfo_t map_info = {"", "", "", 0, 0, 0.f};
PlayerSet_t player_set;

static void on_send	  	(uv_udp_send_t* req, int status);
static void set_id	  	(char* message);
static void get_map_info(char* message);
static void send_text 	(char* buf);
static void make_control_buffer (char** buffer);

static void on_send(uv_udp_send_t* req, int status)
{
	free(req);
}

void alloc_buffer(uv_handle_t* client, size_t sz, uv_buf_t* buf)
{
	buf->base = malloc(sz);
	if (!buf->base) {
		sockets_error = ERROR_ALLOC_FAIL;
		return;
	}

	buf->len = sz;
}

static void send_text(char* buf)
{
	size_t sz = strlen(buf);

	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));
	if (!req) {
		sockets_error = ERROR_ALLOC_FAIL;
		return;
	}

	uv_buf_t wbuf = uv_buf_init(buf, sz);
	uv_req_set_data((uv_req_t*)req, buf);
	uv_udp_send(req, &client_handle, &wbuf, 1, NULL, on_send);
}

static void set_id(char* message)
{
	if (strncmp(message, "id ", 3) == 0)
		player_id = atoi(message + 3);
}

static void get_map_info(char* message)
{
	if (strncmp(message, "map ", 3) != 0)
		return;

	message += 3;
	if (player_id == atoi(message)) {
		sscanf(message, "\n");
		sscanf(message, MAP_INFO, map_info.map, map_info.col, map_info.light,
						          &map_info.px,  &map_info.py,  &map_info.count_coins);
	}
}

void on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
		     const struct sockaddr* addr, unsigned flags)
{
	fprintf(stderr, "recv: %s\n", buf->base);

	if (player_id == -1)
		set_id(buf->base);
	else
		get_map_info(buf->base);
}

void rally_connect()
{
	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));
	if (!req) {
		sockets_error = ERROR_ALLOC_FAIL;
		return;
	}

	char* buf = strdup("connect");
	if (!buf) {
		sockets_error = ERROR_ALLOC_FAIL;
		return;
	}

	size_t sz = strlen(buf);

	uv_buf_t buffer = uv_buf_init(buf, sz);

	uv_udp_send(req, &client_handle, &buffer, 1, NULL, on_send);
}

static void make_control_buffer(char** buffer)
{
	*buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
	if (!(*buffer)) {
		sockets_error = ERROR_ALLOC_FAIL;
		return;
	}

	sprintf(*buffer, PLAYER_CONTROL_FORMAT, player_set.is_exit, player_set.dx, player_set.dy);
}

void on_timer(uv_timer_t* timer)
{
	SDL_PollEvent(&event);

	if (player_id != -1) {
		char* buffer = NULL;
		make_control_buffer(&buffer);
		fprintf(stderr, "%s", buffer);
		send_text(buffer);
	}
}