#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uv.h>
#include <curses.h>
#include <math.h>

#include "Control.h"
#include "Sockets.h"

int main(int argc, char* argv[])
{
	uv_loop_t* loop;	
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
	uv_timer_start(&timer, on_timer, 1500, 1500);

	int result = uv_run(loop, UV_RUN_DEFAULT);
	return result;

}