#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>

#include "ANSI_colors.h"
#include "Life.h"
#include "Input.h"
#include "Errors.h"
#include "Output.h"
#include "ProcessCmd.h"
#include "Window.h"

int main(int argc, const char *argv[])
{
    printf(print_lblue("# Implementation of mini game.\n"
                       "# (c) Baidiusenov Timur, 2024\n\n"));

    srand((unsigned int)time(NULL));
    ErrorCode error = ERROR_NO;
    CmdInputData_t cmd_data = {};

    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    Mix_Music* music = NULL;
    Uint8* pixels = NULL;
    TTF_Font* font;

    Map_t map = {.path = {.path_target = -1}};
    PlayerSet_t PlayerSet = {.is_info = 1, .dx = 1, .dy = 1, .scale = 1.f, .Kscale = 1.3f};

    error = cmd_data_init(argc, argv, &cmd_data);
    if (error) goto error;

    error = cmd_data_callback(&cmd_data);
    if (error) goto error;

    error = cmd_data_verify(&cmd_data);
    if (error) goto error;

    error = objects_get();
    if (error) goto error;
    
    error = map_create(&map, &PlayerSet, cmd_data.map_txt_file);
    if (error) goto error;

    error = window_prepare(&window, &texture, &renderer, &music, &pixels, &font, &cmd_data);
    if (error) goto error;

    error = window_default_loop(&window, &texture, &renderer, &music, pixels, &font, &map, &PlayerSet,
                                cmd_data.screenshot_file);
    if (error) goto error;

    goto finally;

error:
    err_dump(error);

finally:
    cmd_data_delete(&cmd_data);

    printf(print_lblue("\n# Bye!\n"));
    return 0;
}