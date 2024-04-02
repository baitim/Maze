#include <stdio.h>
#include <string.h>
#include <chrono>

#include "Life.h"
#include "Input.h"
#include "Output.h"
#include "Control.h"

void set_text(sf::Font* font, sf::Text* text, float x, float y);
void free_all(sf::Uint8* pixels, char* pos_string, char* fps_string);

int main()
{
    srand((unsigned int)time(NULL));

    //Window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze");
    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);
    // Pixels
    sf::Uint8* pixels = (sf::Uint8*) calloc(sizeof(sf::Uint8), WIDTH * HEIGHT * 4); // (RGBA)
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
    PlayerSet_t PlayerSet = {.is_info = 1, .dx = 1, .dy = 1, .scale = 1.f, .Kscale = 1.2f};
    lab_create(&map, &PlayerSet);

    while (window.isOpen()) {
        auto clock_begin = std::chrono::steady_clock::now();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (control_event(event, map.lab, &PlayerSet)) {
                make_screenshot(&window, "images/Maze.png");
                window.close();
                free_all(pixels, pos_string, fps_string);
                return 0;
            }
        }
        control_noevent(&window, map.lab, &PlayerSet);

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
            snprintf(fps_string, len_fps_string, "fps: %.f ms", fps);
            FPS_Text.setString(fps_string);
            window.draw(FPS_Text);
        }

        window.display();
    }

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