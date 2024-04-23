#ifndef SERVER_SRC_LIFE_H
#define SERVER_SRC_LIFE_H

#include <stdio.h>

#include "Config.h"
#include "Errors.h"

int       pos_in_pix_window (int x, int y);
ErrorCode map_create        (Map_t* map, char* output_file);

#endif // SERVER_SRC_LIFE_H