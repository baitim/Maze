#ifndef SRC_LIFE_H
#define SRC_LIFE_H

#include <stdio.h>

#include "Config.h"
#include "Control.h"
#include "Errors.h"

int       pos_in_pix_window (int x, int y);
ErrorCode map_create        (Map_t* map, PlayerSet_t* PlayerSet, char* output_file);

#endif // SRC_LIFE_H