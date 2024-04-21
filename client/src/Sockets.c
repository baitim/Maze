#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Sockets.h"
#include "Control.h"

#define MAX_SIZE_BUFFER 10000

uv_udp_t client_handle;
int id = -1;

static void on_send	  (uv_udp_send_t* req, int status);
static void set_id	  (char* message);
static void send_text (char* buf);
static void make_control_buffer (char** buffer);

static void on_send(uv_udp_send_t* req, int status)
{
	free(req);
}

void alloc_buffer(uv_handle_t* client, size_t sz, uv_buf_t* buf)
{
	buf->base = malloc(sz);
	buf->len = sz;
}

static void send_text(char* buf)
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

static void make_control_buffer(char** buffer)
{
	PlayerControl_t PlayerControl;
	PlayerControl = (PlayerControl_t){.is_exit = 0, .mouse_x = -1, .mouse_y = -1,
                                       .mouse_button_left = 0, .mouse_button_right = 0,
                                       .dx = 0, .dy = 0, .scale = 0, .space = 0, .info = 0};

	*buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
	sprintf(*buffer, PLAYER_CONTROL_FORMAT,
			PlayerControl.is_exit, PlayerControl.mouse_x, PlayerControl.mouse_y,
			PlayerControl.mouse_button_left, PlayerControl.mouse_button_right,
			PlayerControl.dx, PlayerControl.dy, PlayerControl.scale,
			PlayerControl.space, PlayerControl.info);
}

void on_timer(uv_timer_t* timer)
{
	if (id != -1) {
		char* buffer = NULL;
		make_control_buffer(&buffer);
		fprintf(stderr, "%s", buffer);
		send_text(buffer);
	}
}