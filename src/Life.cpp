#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Life.h"
#include "Math.h"

Object OBJECTS[COUNT_OBJECTS] = {
    {SYM_OBJ_ERR,       COUNT_OBJ_INF,      false,  "images/Texture/TextureError.png",  0,   0, {}},
    {SYM_OBJ_WALL,      COUNT_OBJ_INF,      false,  "images/Texture/TextureWall.png",   20,  0, {}},
    {SYM_OBJ_ROAD,      COUNT_OBJ_INF,      true,   "images/Texture/TextureRoad.png",   5,   0, {}},
    {SYM_OBJ_BORDER,    COUNT_OBJ_INF,      false,  "images/Texture/TextureBorder.png", 100, 0, {}},
    {SYM_OBJ_PLAYER,    COUNT_OBJ_PLAYER,   true,   "images/Texture/TexturePlayer.png", 0,   0, {}},
    {SYM_OBJ_COIN,      COUNT_OBJ_COIN,     true,   "images/Texture/TextureCoin.png",   10,  0, {}},
    {SYM_OBJ_LAMP,      COUNT_OBJ_LAMP,     false,  "images/Texture/TextureLamp.png",   0,   2, {SYM_OBJ_WALL, SYM_OBJ_BORDER}},
    {SYM_OBJ_PATH,      COUNT_OBJ_INF,      true,   "images/Texture/TexturePath.png",    10,  0, {}},
    {SYM_OBJ_DEST,      COUNT_OBJ_INF,      true,   "images/Texture/TextureDest.png",   10,  0, {}},
    {SYM_OBJ_IMDEST,    COUNT_OBJ_INF,      true,   "images/Texture/TextureImDest.png", 10,  0, {}}
};

static void lab_gen    (char* lab);
static void lab_step   (char* lab);
static void lab_fill_empty  (char* lab, PlayerSet_t* PlayerSet);
static void set_lighting    (Map_t* map);
static void set_light_lamp  (Map_t* map, int pos);
static void set_hills       (Map_t* map);
static void paint_subgraph  (int v, int color, int* colors, char* lab);
static int  get_center_graph(int v, int color, int* colors, char* lab);
static void lab_write2file  (char* lab, FILE* f);
static void count_free_pos  (char* lab, int* count_free, int* frees_ind);
static void select_free_pos (char* lab, char* free_pos, int count_free, int* frees_ind);
static bool check_neighbors (Object* src_obj, char* lab, int pos);
static void set_free_pos    (char* lab, PlayerSet_t* PlayerSet);

void lab_create(Map_t* map, PlayerSet_t* PlayerSet, char* output_file)
{
    memset(map->lab,   0, sizeof(char) * BYTE_HEIGHT * BYTE_WIDTH);
    memset(map->light, 0, sizeof(unsigned char) * BYTE_HEIGHT * BYTE_WIDTH);
    memset(map->col,   0, sizeof(unsigned char) * BYTE_HEIGHT * BYTE_WIDTH * 3);
    
    lab_gen(map->lab);

    int x = 0;
    while (x++ < STEPS_GEN)
        lab_step(map->lab);

    lab_fill_empty(map->lab, PlayerSet);

    set_lighting(map);

    // set_hills(map);

    FILE *f = fopen(output_file, "w");
    lab_write2file(map->lab, f);
}

static void set_hills(Map_t* map)
{
    int* colors = (int*) calloc(sizeof(int), BYTE_HEIGHT * BYTE_WIDTH);
    int color   = 1;


    for (int i = 0; i < BYTE_HEIGHT * BYTE_WIDTH; i++) {
        if (map->lab[i] != SYM_OBJ_WALL)
            continue;

        if (!colors[i]) {
            paint_subgraph(i, color, colors, map->lab);
            color++;
        }
    }

    int* visit_color = (int*) calloc(sizeof(int), color);
    int* centers =     (int*) calloc(sizeof(int), color);
    for (int i = 0; i < BYTE_HEIGHT * BYTE_WIDTH; i++) {
        if (map->lab[i] != SYM_OBJ_WALL || colors[i] == 0)
            continue;

        if (!visit_color[colors[i]]) {
            int new_center = get_center_graph(i, color, colors, map->lab);
            fprintf(stderr, "from =   %d\t%d\n", i          / BYTE_WIDTH, i          % BYTE_WIDTH);
            fprintf(stderr, "center = %d\t%d\n", new_center / BYTE_WIDTH, new_center % BYTE_WIDTH);

            centers[colors[i]] = new_center;
        }
    }

    free(centers);
    free(visit_color);
    free(colors);
}

static int get_center_graph(int v, int color, int* colors, char* lab)
{
    // for (int dy = -1; dy <= 1; dy++) {
    //     for (int dx = -1; dx <= 1; dx++) {
    //         if (abs(dx) + abs(dy) != 1)
    //             continue;
            
    //         if (lab[v + dy * BYTE_WIDTH + dx] != SYM_OBJ_WALL)
    //             continue;

    //         paint_subgraph(lab[v + dy * BYTE_WIDTH + dx], color, colors, lab);
    //     }
    // }
    return 0;
}

static void paint_subgraph(int v, int color, int* colors, char* lab)
{
    colors[v] = color;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (abs(dx) + abs(dy) != 1)
                continue;
            
            if (lab[v + dy * BYTE_WIDTH + dx] != SYM_OBJ_WALL)
                continue;

            paint_subgraph(lab[v + dy * BYTE_WIDTH + dx], color, colors, lab);
        }
    }
}

static void set_lighting(Map_t* map)
{
    memset(map->light, 20, BYTE_HEIGHT * BYTE_WIDTH);
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (OBJECTS[k].symbol == map->lab[i * BYTE_WIDTH + j] &&
                    OBJECTS[k].symbol == SYM_OBJ_LAMP) {
                    set_light_lamp(map, i * BYTE_WIDTH + j);
                }
            }
        }
    }
}

static void set_light_lamp(Map_t* map, int pos)
{
    for (int dy = -light_dist; dy <= light_dist; dy++) {
        for (int dx = -light_dist; dx <= light_dist; dx++) {
            if (((pos + dy * BYTE_WIDTH + dx) < 0) || ((pos + dy * BYTE_WIDTH + dx) > BYTE_HEIGHT * BYTE_WIDTH -1))
                continue;

            float dist = sqrtf((float)(dx * dx + dy * dy));
            unsigned char ratio = 255 * (float)(1.f - MIN(1.f, dist / (float)light_dist / light_force));
            unsigned char ratio_norm = MAX(0, MIN(255, ratio));

            unsigned char was_light = map->light[pos + dy * BYTE_WIDTH + dx];
            map->light[pos + dy * BYTE_WIDTH + dx] = (unsigned char) MIN(255, was_light + ratio_norm);

            unsigned char was_red   = map->col[(pos + dy * BYTE_WIDTH + dx) * 3 + 0];
            unsigned char was_green = map->col[(pos + dy * BYTE_WIDTH + dx) * 3 + 1];
            map->col[(pos + dy * BYTE_WIDTH + dx) * 3 + 0] = MIN(255, was_red   + ratio_norm / 7);
            map->col[(pos + dy * BYTE_WIDTH + dx) * 3 + 1] = MIN(255, was_green + ratio_norm / 7);
        }
    }
}

static void lab_gen(char* lab)
{
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            if (byte_board(i, j))
                lab[i * BYTE_WIDTH + j] = SYM_OBJ_BORDER;

            int x = (rand() % 100 + 1);
            if (x >= CHANCE_LIFE)
                lab[i * BYTE_WIDTH + j] = SYM_OBJ_WALL;
            else
                lab[i * BYTE_WIDTH + j] = SYM_OBJ_ROAD;
        }
    }
}

static void lab_step(char* lab)
{
    char new_m[BYTE_HEIGHT * BYTE_WIDTH];
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            if (byte_board(i, j)) {
                new_m[i * BYTE_WIDTH + j] = SYM_OBJ_BORDER;
                continue;
            }
            int byte_val_neighbours = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (!(((dx) != 0) || ((dy) != 0)))
                        continue;
                    if (lab[(i + dx) * BYTE_WIDTH + (j + dy)] == SYM_OBJ_WALL)
                        byte_val_neighbours++;
                }
            }
            if (byte_val_neighbours >= NEW_LIFE)
                new_m[i * BYTE_WIDTH + j] = SYM_OBJ_WALL;
            else if (lab[i * BYTE_WIDTH + j] == SYM_OBJ_WALL && byte_val_neighbours >= STAY_IN_LIFE)
                new_m[i * BYTE_WIDTH + j] = SYM_OBJ_WALL;
            else 
                new_m[i * BYTE_WIDTH + j] = SYM_OBJ_ROAD;
        }
    }
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            lab[i * BYTE_WIDTH + j] = new_m[i * BYTE_WIDTH + j];
        }
    }
}

static void lab_fill_empty(char* lab, PlayerSet_t* PlayerSet)
{
    int count_free = 0;
    int* frees_ind = (int*) calloc(BYTE_HEIGHT * BYTE_WIDTH,  sizeof(int));
    count_free_pos(lab, &count_free, frees_ind);
    
    char* free_pos = (char*) calloc((size_t)count_free, sizeof(char));
    select_free_pos(lab, free_pos, count_free, frees_ind);
    set_free_pos(lab, PlayerSet);

    free(free_pos);
    free(frees_ind);
}

static void count_free_pos(char* lab, int* count_free, int* frees_ind)
{
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (lab[i * BYTE_WIDTH + j] == OBJECTS[k].symbol && OBJECTS[k].can_go) {
                    frees_ind[*count_free] = i * BYTE_WIDTH + j;
                    (*count_free)++;
                }
            }
        }
    }
}

static void select_free_pos(char* lab, char* free_pos, int count_free, int* frees_ind)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (OBJECTS[i].count == COUNT_OBJ_INF) 
            continue;

        OBJECTS[i].pos_num_free = (int*) malloc((size_t)OBJECTS[i].count * sizeof(int));
        for (int j = 0; j < OBJECTS[i].count; j++) {
            int pos = rand() % count_free;

            while (1) {
                if (!free_pos[pos] && check_neighbors(&OBJECTS[i], lab, frees_ind[pos])) {
                    OBJECTS[i].pos_num_free[j] = pos;
                    free_pos[pos] = 1;
                    break;
                }
                pos = rand() % count_free;
            }
        }
    }
}

static bool check_neighbors(Object* src_obj, char* lab, int pos)
{
    if (src_obj->count_neighbors == 0) 
        return true;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (abs(dx) + abs(dy) != 1)
                continue;
            
            for (int i = 0; i < src_obj->count_neighbors; i++) {
                for (int j = 0; j < COUNT_OBJECTS; j++) {
                    if (OBJECTS[j].symbol == lab[pos + dy * BYTE_WIDTH + dx] &&
                        OBJECTS[j].symbol == src_obj->neighbors[i])
                        return true;
                }
            }
        }
    }
    return false;
}

static void set_free_pos(char* lab, PlayerSet_t* PlayerSet)
{
    int count_free = 0;
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            bool skip = false;
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (lab[i * BYTE_WIDTH + j] == OBJECTS[k].symbol && !OBJECTS[k].can_go) {
                    skip = true;
                    break;
                }
            }
            if (skip) continue;

            for (int k = 0; k < COUNT_OBJECTS; k++) {
                bool was = false;
                for (int m = 0; m < OBJECTS[k].count; m++) {
                    if (count_free == OBJECTS[k].pos_num_free[m]) {
                        lab[i * BYTE_WIDTH + j] = OBJECTS[k].symbol;

                        if (OBJECTS[k].symbol == SYM_OBJ_PLAYER) {
                            PlayerSet->px = j;
                            PlayerSet->py = i;
                        }
                        was = true;
                        break;
                    }
                }
                if (was) break;
            }
            count_free++;
        }
    }
}

static void lab_write2file(char* lab, FILE* f) 
{
    int count_free = 0;
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++)
            if (lab[i * BYTE_WIDTH + j] == ' ') count_free++;
    }
    int number_i = rand() % count_free;
    int number_o = rand() % count_free;
    while (number_o == number_i)
         number_o = rand() % count_free;

    count_free = 0;
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++)
            fprintf(f, "%c", lab[i * BYTE_WIDTH + j]);
        fprintf(f, "\n");
    }
}