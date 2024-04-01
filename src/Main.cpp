#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Life.h"
#include "Input.h"
#include "Output.h"
#include "Control.h"

int main()
{
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze");
    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);
    sf::Uint8* pixels = new sf::Uint8[WIDTH * HEIGHT * 4]; // (RGBA)
    memset(pixels, 255, sizeof(sf::Uint8) * WIDTH * HEIGHT * 4);
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text POS_Text;
    POS_Text.setFont(font);
    POS_Text.setPosition(10, 10);
    POS_Text.setCharacterSize(20);
    POS_Text.setColor(sf::Color(252, 0, 17));

    objects_get();

    char lab[N * M];
    XYset_t XYset = {.dx = 1, .dy = 1, .scale = 0.7, .Kscale = 1.2};
    lab_create(lab, &XYset);

    int cycle_counter = 0;
    bool image_saved = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (control_event(&window, event, lab, &XYset)) {
                window.close();
                break;
            }
        }
        control_noevent(&window, lab, &XYset);

        window.clear();
        
        render_lab(pixels, lab, &XYset);
        texture.update(pixels);
        window.draw(sprite);

        if (XYset.is_info) {
            auto pos_string = "POS: " + std::to_string(XYset.px) + "  " + std::to_string(XYset.py);
            POS_Text.setString(pos_string);
            window.draw(POS_Text);
        }

        window.display();

        if (cycle_counter == 3 && !image_saved) {
            sf::Image image = window.capture();
            image.saveToFile("images/Maze.png");
            fprintf(stderr, "Image was saved to file\n");
            image_saved = true;
        }
        cycle_counter = (cycle_counter + 1) % 100;
    }

    return 0;
}