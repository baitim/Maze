#ifndef CLIENT_SRC_SOCKETS_H
#define CLIENT_SRC_SOCKETS_H

#include <uv.h>

#include "Config.h"
#include "Errors.h"

#define MAP_INFO "MapInfo {\n"    		\
                 "\tmap: %s[^Ъ]\n"    	\
                 "\tcol: %s[^Ъ]\n"   	\
                 "\tlight: %s[^Ъ]\n"	\
                 "\tpx: %d\n"  			\
                 "\tpy: %d\n"  			\
				 "\tcount_coins: %d\n"  \
                 "}\n"

#define PLAYER_CONTROL_FORMAT "PlayerControl {\n"   \
                              "\tis_exit: %d\n"     \
                              "\tdx: %d\n"          \
                              "\tdy: %d\n"          \
                              "}\n"                 

extern uv_udp_t client_handle;
extern int player_id;
extern ErrorCode sockets_error;

void alloc_buffer	(uv_handle_t* client, size_t sz, uv_buf_t* buf);
void rally_connect	();
void on_recv        (uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
		             const struct sockaddr* addr, unsigned flags);
void on_timer       (uv_timer_t* timer);

#endif // CLIENT_SRC_SOCKETS_H