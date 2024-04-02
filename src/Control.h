#ifndef CONTROL_H
#define CONTROL_H

#include <SFML/Graphics.hpp>

#include "Config.h"

struct PlayerSet_t {
    bool is_active_mouse;
    bool is_info;
    int px, py;
    int dx, dy;
    int delay_dx, delay_dy;
    float scale, Kscale;
};

int control_event  (sf::Event event, char* lab, PlayerSet_t* PlayerSet);
int control_noevent(sf::RenderWindow* window, char* lab, PlayerSet_t* PlayerSet);

#endif // CONTROL_H