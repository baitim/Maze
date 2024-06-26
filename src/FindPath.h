#ifndef SRC_FINDPATH_H
#define SRC_FINDPATH_H

#include "Config.h"
#include "Control.h"
#include "Errors.h"

void      clean_path         (Path_t* path);
ErrorCode find_shortest_path (Map_t* map, PlayerSet_t* PlayerSet, int mouse_x, int mouse_y);

#endif // SRC_FINDPATH_H