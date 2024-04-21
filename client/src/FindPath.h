#ifndef CLIENT_SRC_FINDPATH_H
#define CLIENT_SRC_FINDPATH_H

#include "Config.h"
#include "Control.h"
#include "Errors.h"

void      clean_path         (Path_t* path);
ErrorCode find_shortest_path (int mouse_x, int mouse_y);

#endif // CLIENT_SRC_FINDPATH_H