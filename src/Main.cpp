#include <stdio.h>
#include <string.h>
#include <chrono>

#include "ANSI_colors.h"
#include "Life.h"
#include "Input.h"
#include "Errors.h"
#include "Output.h"
#include "ProcessCmd.h"
#include "Control.h"

void free_all(sf::Uint8* pixels, char* pos_string, char* fps_string);

int main(int argc, const char *argv[])
{
    printf(print_lblue("# Implementation of mini game.\n"
                       "# (c) Baidiusenov Timur, 2024\n\n"));

    srand((unsigned int)time(NULL));
    ErrorCode error = ERROR_NO;
    CmdInputData_t cmd_data = {};

    error = input_cmd(argc, argv, &cmd_data);
    if (error) return error;

    if (cmd_data.is_help)
        print_help();

    if (!cmd_data.is_output_file) {
        error = ERROR_INVALID_FILE;
        return error;
    }

    //Window
    sf::RenderWindow window(sf::VideoMode(PIX_WIDTH, PIX_HEIGHT), "Maze");
    sf::Texture texture;
    texture.create(PIX_WIDTH, PIX_HEIGHT);
    sf::Sprite sprite(texture);
    // Pixels
    sf::Uint8* pixels = (sf::Uint8*) calloc(sizeof(sf::Uint8), PIX_WIDTH * PIX_HEIGHT * 4); // (RGBA)
    // FONT
    sf::Font font;
    font.loadFromFile("arial.ttf");
    
    // Text
    sf::Text POS_Text;
    set_text(&font, &POS_Text, 10, 10);
    int len_pos_string = MAX_SIZE_INFO_STR;
    char* pos_string = (char*) calloc(len_pos_string, sizeof(char));

    sf::Text FPS_Text;
    set_text(&font, &FPS_Text, 10, 45);
    int len_fps_string = MAX_SIZE_INFO_STR;
    char* fps_string = (char*) calloc(len_fps_string, sizeof(char));

    objects_get();

    Map_t map = {};
    PlayerSet_t PlayerSet = {.is_info = 1, .dx = 1, .dy = 1, .scale = 1.f, .Kscale = 2.f};
    lab_create(&map, &PlayerSet, cmd_data.output_file);

    while (window.isOpen()) {
        auto clock_begin = std::chrono::steady_clock::now();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (control_event(&window, &map, &PlayerSet, &event)) {
                make_screenshot(&window, "images/Maze.png");
                window.close();
                free_all(pixels, pos_string, fps_string);
                return 0;
            }
        }
        control_noevent(&window, &map, &PlayerSet);

        window.clear();
        render_lab(pixels, &map, &PlayerSet);
        texture.update(pixels);
        window.draw(sprite);

        auto clock_end = std::chrono::steady_clock::now();
        if (PlayerSet.is_info) {
            snprintf(pos_string, len_pos_string, "pos: %d  %d", PlayerSet.px, PlayerSet.py);
            POS_Text.setString(pos_string);
            window.draw(POS_Text);

            auto elapsed_ms = std::chrono::duration<double,std::milli>(clock_end - clock_begin).count();
            double fps = 100.f / elapsed_ms;
            snprintf(fps_string, len_fps_string, "fps: %.f", fps);
            FPS_Text.setString(fps_string);
            window.draw(FPS_Text);
        }

        window.display();
    }

    goto finally;

error:
    err_dump(error);

finally:
    free_all(pixels, pos_string, fps_string);
    return 0;
}

void free_all(sf::Uint8* pixels, char* pos_string, char* fps_string)
{
    free(pixels);
    free(pos_string);
    free(fps_string);

    for (int i = 0; i < COUNT_OBJECTS; i++) {
        free(OBJECTS[i].pos_num_free);
    }
}