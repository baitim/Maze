#ifndef CONTROL_H
#define CONTROL_H

#include <SFML/Graphics.hpp>

#include "Config.h"

struct XYset_t {
    int px, py;
    int dx, dy;
};

int control(sf::RenderWindow* window, sf::Event event, char* lab, XYset_t* XYset);

#endif // CONTROL_H