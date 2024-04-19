#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FindPath.h"
#include "Math.h"
#include "Queue.h"

static ErrorCode step        (int* distance, char* map, int x, int y, Queue_t** queue);
static void is_step_par      (int* distance, char* map, int* x, int* y);
static int  set_shortest_path(int* distance, Map_t* map, PlayerSet_t* PlayerSet,
                              int mouse_x, int mouse_y, Path_t* path);

static ErrorCode step(int* distance, char* map, int x, int y, Queue_t** queue)
{
    ErrorCode error = ERROR_NO;

    int len = distance[BYTE_WIDTH * y + x];
    int* new_queue;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((abs(dx) + abs(dy) == 1) && passable_object(map, x + dx, y + dy)) {
                new_queue = &distance[BYTE_WIDTH * (y + dy) + x + dx];

                if (*new_queue == 0 || *new_queue > len + 1) {
                    *new_queue = len + 1;
                    error = enqueue(queue, x + dx, y + dy);
                    if (error) return error;
                }
            }
        }
    }
    return ERROR_NO;
}

static void is_step_par(int* distance, char* map, int* x, int* y)
{
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (((dx != 0) || (dy != 0)) && passable_object(map, *x + dx, *y + dy)) {
                if (distance[BYTE_WIDTH * (*y + dy) + *x + dx] + 1 - distance[BYTE_WIDTH * (*y) + *x] == 0) {
                    *x += dx;
                    *y += dy;
                    return;
                }
            }
        }
    }
}

static int set_shortest_path(int* distance, Map_t* map, PlayerSet_t* PlayerSet,
                             int mouse_x, int mouse_y, Path_t* path)
{
    int x = mouse_x, y = mouse_y;
    while (x != PlayerSet->px || y != PlayerSet->py) {
        int old_x = x;
        int old_y = y;
        is_step_par(distance, map->map, &x, &y);
        path->count++;
        path->path[BYTE_WIDTH * old_y + old_x] = path->count;
        if (old_x == x && old_y == y) return 0;
    }
    return 1;
}

void clean_path(Path_t* path)
{
    memset(path->path, 0, BYTE_HEIGHT * BYTE_WIDTH * sizeof(int));
        
    path->count       = 0;
    path->passed      = 0;
    path->path_exist  = 0;
    path->path_target = -1;
}

ErrorCode find_shortest_path(Map_t* map, PlayerSet_t* PlayerSet, int mouse_x, int mouse_y)
{
    ErrorCode error = ERROR_NO;
    Queue_t* queue = NULL;

    clean_path(&map->path);

    map->path.path_target = BYTE_WIDTH * mouse_y + mouse_x;
    
    if (!passable_object(map->map, mouse_x, mouse_y))
        return ERROR_NO;

    int distance[BYTE_HEIGHT * BYTE_WIDTH] = {};

    distance[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = 1;
    error = enqueue(&queue, PlayerSet->px, PlayerSet->py);
    if (error) return error;

    while (queue) {
        int x, y;
        if (!dequeue(&queue, &x, &y)) {
            if (x == mouse_x && y == mouse_y) {
                map->path.path_exist = 1;
                break;
            }
            error = step(distance, map->map, x, y, &queue);
            if (error) return error;
        } else {
            map->path.path_exist = 0;
            return ERROR_NO;
        }
    }
    dtor_queue(queue, queue);

    int is_path_exist = set_shortest_path(distance, map, PlayerSet, mouse_x, mouse_y, &map->path);
    if (!is_path_exist) {
        clean_path(&map->path);
        map->path.path_target = BYTE_WIDTH * mouse_y + mouse_x;
        return ERROR_NO;
    }

    for (int i = 0; i < BYTE_HEIGHT * BYTE_WIDTH; i++)
        if (map->path.path[i] != 0)
            map->path.path[i] = map->path.count - map->path.path[i] + 1;

    return ERROR_NO;
}