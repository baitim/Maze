#include <chrono>
#include <string.h>

#include "Output.h"
#include "Window.h"

static void free_window_stuff(sf::Uint8* pixels, char* pos_string, char* fps_string);

ErrorCode window_prepare(sf::RenderWindow* window, sf::Texture* texture, sf::Sprite* sprite,
                         sf::Uint8** pixels, sf::Font* font, char* font_file,
                         sf::Text* POS_Text, sf::Text* FPS_Text)
{
    window->create(sf::VideoMode(PIX_WIDTH, PIX_HEIGHT), "Maze");

    texture->create(PIX_WIDTH, PIX_HEIGHT);
    sprite->setTexture(*texture);

    *pixels = (sf::Uint8*) calloc(sizeof(sf::Uint8), PIX_WIDTH * PIX_HEIGHT * 4);
    if (!(*pixels)) return ERROR_ALLOC_FAIL;

    font->loadFromFile(font_file);

    set_text(font, POS_Text, 10, 10);
    set_text(font, FPS_Text, 10, 45);

    return ERROR_NO;
}

ErrorCode window_default_loop(sf::RenderWindow* window, sf::Texture* texture, sf::Sprite* sprite,
                              sf::Uint8* pixels, sf::Text* POS_Text, sf::Text* FPS_Text,
                              Map_t* map, PlayerSet_t* PlayerSet, char* screenshot_file)
{
    int len_pos_string = MAX_SIZE_INFO_STR;
    char* pos_string = (char*) calloc(len_pos_string, sizeof(char));
    if (!pos_string) return ERROR_ALLOC_FAIL;

    int len_fps_string = MAX_SIZE_INFO_STR;
    char* fps_string = (char*) calloc(len_fps_string, sizeof(char));
    if (!fps_string) return ERROR_ALLOC_FAIL;
    
    while (window->isOpen()) {
        auto clock_begin = std::chrono::steady_clock::now();

        sf::Event event;
        while (window->pollEvent(event)) {
            if (control_event(window, map, PlayerSet, &event)) {
                make_screenshot(window, screenshot_file);
                window->close();
                free_window_stuff(pixels, pos_string, fps_string);
                return ERROR_NO;
            }
        }
        control_noevent(window, map, PlayerSet);

        window->clear();
        render_lab(pixels, map, PlayerSet);
        texture->update(pixels);
        window->draw(*sprite);

        auto clock_end = std::chrono::steady_clock::now();
        if (PlayerSet->is_info) 
            print_state_info(window, POS_Text, FPS_Text, pos_string, len_pos_string,
                             fps_string, len_fps_string, clock_begin, clock_end, PlayerSet);

        window->display();
    }

    free_window_stuff(pixels, pos_string, fps_string);
    return ERROR_NO;
}

static void free_window_stuff(sf::Uint8* pixels, char* pos_string, char* fps_string)
{
    free(pixels);
    free(pos_string);
    free(fps_string);

    for (int i = 0; i < COUNT_OBJECTS; i++) {
        free(OBJECTS[i].pos_num_free);
    }
}