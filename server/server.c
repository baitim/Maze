#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_PLAYERS 5000
#define MAX_SIZE_BUFFER 1000

typedef struct _player {
	int status;
	struct sockaddr_in addr;
	uv_udp_t sock;
	int num;
	float px, py;
} Player;

Player players[MAX_PLAYERS];

uv_loop_t* loop;
uv_udp_t server;

void register_player(const struct sockaddr* addr);

void send_text_cb(uv_udp_send_t* req, int status)
{
	free(uv_req_get_data((uv_req_t*)req));
	free(req);
}

void send_text(int i, char* buf)
{
	size_t sz = strlen(buf);

	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));
	uv_buf_t wbuf = uv_buf_init(buf, sz);
	uv_req_set_data((uv_req_t*)req, buf);
	uv_udp_send(req, &server, &wbuf, 1, (struct sockaddr*)&players[i].addr,
			send_text_cb);
}

void alloc_buffer(uv_handle_t* client, size_t sz, uv_buf_t* buf)
{
	buf->base = malloc(sz);
	buf->len = sz;
}

void answer_message(const struct sockaddr* addr)
{
	char* buf = calloc(MAX_SIZE_BUFFER, sizeof(char));
	memcpy(buf, "message get", 11);

	size_t sz = strlen(buf);

	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));
	uv_buf_t wbuf = uv_buf_init(buf, sz);
	uv_req_set_data((uv_req_t*)req, buf);
	uv_udp_send(req, &server, &wbuf, 1, (struct sockaddr*)addr,
			send_text_cb);
}

void on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
			 const struct sockaddr* addr, unsigned flags)
{
	if (nread > 0) {
		printf("%ld bytes got from %p\n", nread, addr);
		printf("message = %s\n", buf->base);
		if (!strncmp(buf->base, "connect", 7) ) {
			printf("Registering...\n");
			register_player(addr);
		} else if (nread > 7 && !strncmp(buf->base, "msg ", 4) ) {
			answer_message(addr);
		} else {
			printf("Unknown request\n");
		}
	}
}

void on_timer(uv_timer_t* timer)
{
	for (int i = 0; i < MAX_PLAYERS; i++ ) {
		if (!players[i].status) {
			char* buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
			memcpy(buffer, "TIMER TICK", 10);
			send_text(i, buffer);
		}
	}
}

void register_player(const struct sockaddr* addr)
{
	for (int i = 0; i < MAX_PLAYERS; i++ ) {
		if (!players[i].status) {
			struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
			char* a= (char*)&(addr_in->sin_addr.s_addr);
			printf("Registering player from %hhu.%hhu.%hhu.%hhu:%d\n", a[0], a[1], a[2], a[3], addr_in->sin_port);
			players[i] = (Player){1, *addr_in, (uv_udp_t){0}, i, 7, 11};
			uv_udp_init(loop, &players[i].sock);
			
			char* buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
			sprintf(buffer, "id %d", i);
			send_text(i, buffer);
			return;
		}
	}
}

int main()
{
	uv_timer_t timer;
	struct sockaddr_in addr;
	loop = uv_default_loop();

	uv_udp_init(loop, &server);
	uv_ip4_addr("192.168.3.10", 9973, &addr);
	uv_udp_bind(&server, (const struct sockaddr*)&addr, 0);
	printf("Starting server\n");
	uv_udp_recv_start(&server, alloc_buffer, on_recv);
	
	uv_timer_init(loop, &timer);
	printf("Starting timer\n");
	uv_timer_start(&timer, on_timer, 2000, 2000);

	int result = uv_run(loop, UV_RUN_DEFAULT);
	return result;

}
