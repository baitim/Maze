#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Life.h"
#include "Input.h"
#include "Output.h"
#include "Control.h"

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

    // TEXT
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text POS_Text;
    POS_Text.setFont(font);
    POS_Text.setPosition(10, 10);
    POS_Text.setCharacterSize(28);
    POS_Text.setOutlineColor(sf::Color(252, 0, 17));

    objects_get();

    Map_t map = {};
    PlayerSet_t PlayerSet = {.is_info = 1, .dx = 1, .dy = 1, .scale = 1.f, .Kscale = 1.2f};
    lab_create(&map, &PlayerSet);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (control_event(event, map.lab, &PlayerSet)) {
                make_screenshot(&window, "images/Maze.png");
                window.close();
                break;
            }
        }
        control_noevent(&window, map.lab, &PlayerSet);

        window.clear();
        render_lab(pixels, &map, &PlayerSet);
        texture.update(pixels);
        window.draw(sprite);

        if (PlayerSet.is_info) {
            auto pos_string = "POS: " + std::to_string(PlayerSet.px) + "  " + std::to_string(PlayerSet.py);
            POS_Text.setString(pos_string);
            window.draw(POS_Text);
        }

        window.display();
    }

    return 0;
}