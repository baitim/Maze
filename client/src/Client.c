#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uv.h>
#include <curses.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "ANSI_colors.h"
#include "Control.h"
#include "Sockets.h"
#include "ProcessCmd.h"
#include "Window.h"
#include "Output.h"
#include "Input.h"

int main(int argc, const char* argv[])
{
	printf(print_lblue("# Implementation of mini game.\n"
                       "# (c) Baidiusenov Timur, 2024\n\n"));

    srand((unsigned int)time(NULL));
    ErrorCode error = ERROR_NO;
    CmdInputData_t cmd_data = {};
    InfoStrings_t info_strings = {};
    SDL_Window* window      = NULL;
    SDL_Texture* texture    = NULL;
    SDL_Renderer* renderer  = NULL;
    Mix_Music* music        = NULL;
    Uint8* pixels           = NULL;
    TTF_Font* font          = NULL;
    uv_loop_t* loop         = NULL;
	uv_timer_t timer;
	struct sockaddr_in addr;
    DataOnRecv_t* data_on_recv = NULL;

	error = cmd_data_init(argc, argv, &cmd_data);
    if (error) goto error;

    error = cmd_data_verify(&cmd_data);
    if (error) goto error;

    error = cmd_data_callback(&cmd_data);
    if (error) goto error;

    error = objects_get();
    if (error) goto error;

    error = info_strings_init(&info_strings);
    if (error) goto error;

    error = window_prepare(&window, &texture, &renderer, &music, &pixels, &font, &cmd_data);
    if (error) goto error;

    data_on_recv = malloc(sizeof(DataOnRecv_t));
	if (!data_on_recv) {
		error = ERROR_ALLOC_FAIL;
		goto error;
	}
    data_on_recv->window    = &window;
    data_on_recv->texture   = &texture;
    data_on_recv->renderer  = &renderer;
    data_on_recv->music     = &music;
    data_on_recv->pixels    = &pixels;
    data_on_recv->font      = &font;
    data_on_recv->screenshot_file = cmd_data.screenshot_file;
    data_on_recv->info_strings = &info_strings;
	uv_handle_set_data((uv_handle_t*)&addr, (void*)data_on_recv);

	loop = uv_default_loop();
	uv_udp_init(loop, &client_handle);
	uv_ip4_addr("0.0.0.0", 5002, &addr);

	uv_udp_connect(&client_handle,  (const struct sockaddr*)&addr);
	uv_udp_recv_start(&client_handle, alloc_buffer, on_recv);
	rally_connect();

	uv_timer_init(loop, &timer);
	uv_timer_start(&timer, on_timer, 1500, 1500);
    int result = uv_run(loop, UV_RUN_DEFAULT);

error:
    err_dump(error);

finally:
    cmd_data_delete    (&cmd_data);
    info_strings_delete(&info_strings);
    free_window_stuff  (window, renderer, texture, font, music, pixels);

    printf(print_lblue("\n# Bye!\n"));
	return 0;
}