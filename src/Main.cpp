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
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");
    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);
    sf::Uint8* pixels = new sf::Uint8[WIDTH * HEIGHT * 4]; // (RGBA)
    memset(pixels, 255, sizeof(sf::Uint8) * WIDTH * HEIGHT * 4);
    
    objects_get();

    XYset_t XYset = {.px = 0, .py = 0, .dx = 1, .dy = 1};

    char lab[N * M];
    lab_create(lab, &XYset);

    fprintf (stderr, "px = %d\tpy = %d\n", XYset.px, XYset.py);

    int cycle_counter = 0;
    bool image_saved = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (control(&window, event, lab, &XYset)) {
                window.close();
                break;
            }
        }

        window.clear();

        render_lab(pixels, lab, &XYset);
        texture.update(pixels);
        window.draw(sprite);

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