#include <stdlib.h>
#include <string.h>

#include "Sockets.h"
#include "Config.h"
#include "Control.h"

#define MAX_SIZE_BUFFER 1000000

uv_loop_t* loop;
uv_udp_t server;
PlayerSet_t players[MAX_PLAYERS];

static void send_text_cb    (uv_udp_send_t* req, int status);
static void send_text       (int i, char* buf);
static void send_map        (int i, DataOnTimer_t* data_on_timer);
static void answer_message  (const struct sockaddr* addr, int id);
static void get_player_state(char* message, int id, DataOnRecv_t* data_on_recv);
static void process_player  (const uv_buf_t* buf, const struct sockaddr* addr, DataOnRecv_t* data_on_recv);
static void register_player (const struct sockaddr* addr, DataOnRecv_t* data_on_recv);

static void send_text_cb(uv_udp_send_t* req, int status)
{
	free(uv_req_get_data((uv_req_t*)req));
	free(req);
}

static void send_text(int i, char* buf)
{
	size_t sz = strlen(buf);

	uv_udp_send_t* req = malloc(sizeof(uv_udp_send_t));
	if (!req) return;

	uv_buf_t wbuf = uv_buf_init(buf, sz);
	uv_req_set_data((uv_req_t*)req, buf);
	uv_udp_send(req, &server, &wbuf, 1, (struct sockaddr*)&players[i].addr, send_text_cb);
}

void alloc_buffer(uv_handle_t* client, size_t sz, uv_buf_t* buf)
{
	buf->base = malloc(sz);
	buf->len = sz;
}

static void get_player_state(char* message, int id, DataOnRecv_t* data_on_recv)
{
	sscanf(message, PLAYER_CONTROL_FORMAT_IN, &players[id].is_exit, &players[id].dx, &players[id].dy);
}

static void process_player(const uv_buf_t* buf, const struct sockaddr* addr, DataOnRecv_t* data_on_recv)
{
	int id = 0;
	int nread = 0;
	if (sscanf(buf->base, "%d\n%n", &id, &nread) == 1) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (players[i].status == PLAYER_STATUS_EXIT) continue;

			if (players[i].id == id && memcmp(&players[i].addr, addr, sizeof(struct sockaddr_in) != 0)) {
				printf("Unauthorized access!\n");
				return;
			} 
			else if (players[i].id == id) {
				get_player_state(buf->base, id, data_on_recv);
				return;
			}
		}
	} else {
		printf("Error while parsing, message: message format invalid\n");
	}
}

void on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
			 const struct sockaddr* addr, unsigned flags)
{
	DataOnRecv_t* data_on_recv = (DataOnRecv_t*)uv_handle_get_data((uv_handle_t*)handle);

	if (nread > 0) {
		printf("%ld bytes got from %p\n", nread, addr);
		printf("message = %s\n", buf->base);
		if (!strncmp(buf->base, "connect", 7)) {
			printf("Registering...\n");
			register_player(addr, data_on_recv);
		} else if (!strncmp(buf->base, "player ", 7)) {
			process_player(buf, addr, data_on_recv);
		} else {
			printf("Unknown request\n");
		}
	}
}

static void send_state(int i, DataOnTimer_t* data_on_timer)
{
	char* buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
	sprintf(buffer, "map %d\n", i);

	sprintf(buffer, MAP_INFO_OUT, data_on_timer->map, data_on_timer->col,
								  data_on_timer->light, players[i].px, players[i].py,
								  players[i].count_coins);

	send_text(i, buffer);
}

void on_timer(uv_timer_t* timer)
{
	DataOnTimer_t* data_on_timer = (DataOnTimer_t*)uv_handle_get_data((uv_handle_t*)timer);

	for (int i = 0; i < MAX_PLAYERS; i++ ) {
		if (players[i].status == PLAYER_STATUS_EXIST) {
			send_state(i, data_on_timer);
		}
	}
}

static void register_player(const struct sockaddr* addr, DataOnRecv_t* data_on_recv)
{

	for (int i = 0; i < MAX_PLAYERS; i++ ) {
		if (players[i].status == PLAYER_STATUS_EXIT) {
			struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
			char* a= (char*)&(addr_in->sin_addr.s_addr);
			printf("Registering player from %hhu.%hhu.%hhu.%hhu:%d\n", a[0], a[1], a[2], a[3], addr_in->sin_port);
			players[i] = (PlayerSet_t){PLAYER_STATUS_EXIST, 0, 0, 0, 0, 0, 0, i,
			 						   *addr_in, (uv_udp_t){0}};

			set_player_start_pos(&players[i], data_on_recv->map);

			uv_udp_init(loop, &players[i].sock);
			
			char* buffer = calloc(MAX_SIZE_BUFFER, sizeof(char));
			sprintf(buffer, "id %d", i);
			send_text(i, buffer);
			return;
		}
	}
}