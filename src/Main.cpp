#include <stdio.h>

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

    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Uint8* pixels = NULL;
    sf::Font font;
    sf::Text POS_Text, FPS_Text;

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

    error = window_prepare(&window, &texture, &sprite, &pixels, &font, cmd_data.font_file,
                           &POS_Text, &FPS_Text);
    if (error) goto error;

    error = window_default_loop(&window, &texture, &sprite, pixels, &POS_Text, &FPS_Text,
                                &map, &PlayerSet, cmd_data.screenshot_file);
    if (error) goto error;

    goto finally;

error:
    err_dump(error);

finally:
    cmd_data_delete(&cmd_data);

    printf(print_lblue("\n# Bye!\n"));
    return 0;
}