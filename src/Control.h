#ifndef SRC_CONTROL_H
#define SRC_CONTROL_H

#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Errors.h"

typedef struct PlayerSet_t_ {
    bool is_active_mouse_move;
    bool is_active_mouse_click;
    bool is_info;
    int px, py;
    int dx, dy;
    int delay_dx, delay_dy;
    int delay_path;
    int delay_info;
    float scale, Kscale;
} PlayerSet_t;

ErrorCode control_event  (sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet,
                          sf::Event* event, int* is_exit);
void      control_noevent(sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet);
int       passable_object(char* map, int x, int y);

#endif // SRC_CONTROL_H