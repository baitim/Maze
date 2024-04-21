#ifndef CLIENT_SOCKETS_H
#define CLIENT_SOCKETS_H

#include <uv.h>

extern uv_udp_t client_handle;
extern int id;

void alloc_buffer	(uv_handle_t* client, size_t sz, uv_buf_t* buf);
void rally_connect	();
void on_recv        (uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
		             const struct sockaddr* addr, unsigned flags);
void on_timer       (uv_timer_t* timer);

#endif // CLIENT_SOCKETS_H