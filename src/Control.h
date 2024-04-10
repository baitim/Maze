#ifndef SRC_CONTROL_H
#define SRC_CONTROL_H

#include <SFML/Graphics.hpp>

#include "Config.h"

struct PlayerSet_t {
    bool is_active_mouse_move;
    bool is_active_mouse_click;
    bool is_info;
    int px, py;
    int dx, dy;
    int delay_dx, delay_dy;
    int delay_path;
    float scale, Kscale;
};

int control_event  (sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet, sf::Event* event);
int control_noevent(sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet);
int passable_object(char* lab, int x, int y);

#endif // SRC_CONTROL_H