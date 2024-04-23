#ifndef SERVER_SRC_SOCKETS_H
#define SERVER_SRC_SOCKETS_H

#include "uv.h"

#include "Config.h"

typedef struct DataOnTimer_t_ {
	char map[SCREEN_BYTES_COUNT];
    unsigned char col[SCREEN_BYTES_COUNT * 3];
    unsigned char light[SCREEN_BYTES_COUNT];
} DataOnTimer_t;

typedef struct DataOnRecv_t_ {
	char map[SCREEN_BYTES_COUNT];
	int is_exist;
	int dx, dy;
} DataOnRecv_t;

#define MAP_INFO_OUT	"MapInfo {\n"    		\
                    	"\tmap: %sЪ\n"    	\
                    	"\tcol: %sЪ\n"   	\
                    	"\tlight: %sЪ\n"	    \
                    	"\tpx: %d\n"  			\
                    	"\tpy: %d\n"  			\
				    	"\tcount_coins: %d\n"   \
                    	"}\n"

#define PLAYER_CONTROL_FORMAT_IN   	"PlayerControl {\n"   \
                                    "\tis_exit: %d\n"     \
                                    "\tdx: %d\n"          \
                                    "\tdy: %d\n"          \
                                    "}\n"  

extern uv_loop_t* loop;
extern uv_udp_t server;

void alloc_buffer   (uv_handle_t* client, size_t sz, uv_buf_t* buf);
void on_recv        (uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
			         const struct sockaddr* addr, unsigned flags);
void on_timer       (uv_timer_t* timer);

#endif // SERVER_SRC_SOCKETS_H