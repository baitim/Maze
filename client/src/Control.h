#ifndef CLIENT_SRC_CONTROL_H
#define CLIENT_SRC_CONTROL_H

#include <SDL2/SDL.h>

#include "Sockets.h"

void control        ();
void control_noevent();
int passable_object (char* map, int x, int y);

#endif // CLIENT_SRC_CONTROL_H