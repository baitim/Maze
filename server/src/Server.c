#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uv.h>

#include "Life.h"
#include "Config.h"
#include "Sockets.h"
#include "Errors.h"
#include "ProcessCmd.h"

int main(int argc, const char* argv[])
{
	srand((unsigned int)time(NULL));
    ErrorCode error = ERROR_NO;
    CmdInputData_t cmd_data = {};

	uv_timer_t timer;
	struct sockaddr_in addr;
	int loop_result = 0;

	error = cmd_data_init(argc, argv, &cmd_data);
    if (error) goto error;

    error = cmd_data_verify(&cmd_data);
    if (error) goto error;

    error = cmd_data_callback(&cmd_data);
    if (error) goto error;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		players[i].status 		= PLAYER_STATUS_EXIT;
		players[i].is_exit 		= 0;
		players[i].count_coins 	= 0;
		players[i].px 			= 0;
		players[i].py 			= 0;
		players[i].dx 			= 0;
		players[i].dy 			= 0;
	}

	Map_t map = {};

    error = map_create(&map, cmd_data.map_txt_file);
    if (error) goto error;

	loop = uv_default_loop();

	DataOnRecv_t* data_on_recv = malloc(sizeof(DataOnRecv_t));
	if (!data_on_recv) {
		error = ERROR_ALLOC_FAIL;
		goto error;
	}
	memcpy(data_on_recv->map, map.map, SCREEN_BYTES_COUNT * sizeof(char));
	uv_handle_set_data((uv_handle_t*)&addr, (void*)data_on_recv);

	uv_udp_init(loop, &server);
	uv_ip4_addr("0.0.0.0", 5002, &addr);
	uv_udp_bind(&server, (const struct sockaddr*)&addr, 0);
	printf("Starting server\n");
	uv_udp_recv_start(&server, alloc_buffer, on_recv);
	
	DataOnTimer_t* data_on_timer = malloc(sizeof(DataOnTimer_t));
	if (!data_on_timer) {
		error = ERROR_ALLOC_FAIL;
		goto error;
	}
	memcpy(data_on_timer->map,   map.map,   SCREEN_BYTES_COUNT * sizeof(char));
	memcpy(data_on_timer->col,   map.col,   SCREEN_BYTES_COUNT * 3 * sizeof(char));
	memcpy(data_on_timer->light, map.light, SCREEN_BYTES_COUNT * sizeof(unsigned char));
	uv_handle_set_data((uv_handle_t*)&timer, (void*)data_on_timer);

	uv_timer_init(loop, &timer);
	printf("Starting timer\n");
	uv_timer_start(&timer, on_timer, 1000, 1000);

	loop_result = uv_run(loop, UV_RUN_DEFAULT);

error:
    err_dump(error);

finally:
    cmd_data_delete(&cmd_data);

	return 0;
}
