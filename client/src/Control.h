#ifndef CLIENT_CONTROL_H
#define CLIENT_CONTROL_H

#include <SDL2/SDL.h>

#define PLAYER_CONTROL_FORMAT "PlayerControl {\n"           \
                              "\tis_exit: %d\n"             \
                              "\tmouse_x: %d\n"             \
                              "\tmouse_y: %d\n"             \
                              "\tmouse_button_left: %d\n"   \
                              "\tmouse_button_right: %d\n"  \
                              "\tdx: %d\n"                  \
                              "\tdy: %d\n"                  \
                              "\tscale: %d\n"               \
                              "\tspace: %d\n"               \
                              "\tinfo: %d\n"

typedef struct PlayerControl_t_ {
    int is_exit;
    int mouse_x, mouse_y;
    int mouse_button_left;
    int mouse_button_right;
    int dx, dy;
    int scale;
    int space;
    int info;
} PlayerControl_t;

void control(SDL_Event* event, PlayerControl_t* PlayerControl);

#endif // CLIENT_CONTROL_H