#ifndef CONTROL_H
#define CONTROL_H

#include <SFML/Graphics.hpp>

#include "Config.h"

struct XYset_t {
    bool is_active_mouse;
    bool is_info;
    int px, py;
    int dx, dy;
    int delay_dx, delay_dy;
    float scale, Kscale;
};

int control_event  (sf::RenderWindow* window, sf::Event event, char* lab, XYset_t* XYset);
int control_noevent(sf::RenderWindow* window, char* lab, XYset_t* XYset);

#endif // CONTROL_H