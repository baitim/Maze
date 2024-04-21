#ifndef SRC_CONTROL_H
#define SRC_CONTROL_H

#include <SDL2/SDL.h>

#include "Config.h"
#include "Errors.h"

typedef struct PlayerSet_t_ {
    int is_active_mouse_move;
    int is_active_mouse_click;
    int is_info;
    int px, py;
    int dx, dy;
    int delay_dx, delay_dy;
    int delay_path;
    int delay_info;
    float scale, Kscale;
    int count_coins;
} PlayerSet_t;

ErrorCode control_event  (Map_t* map, PlayerSet_t* PlayerSet, SDL_Event* event, int* is_exit, SDL_Renderer** renderer);
void      control_noevent(Map_t* map, PlayerSet_t* PlayerSet, SDL_Renderer** renderer);
int       passable_object(char* map, int x, int y);

#endif // SRC_CONTROL_H