#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Life.h"

Object OBJECTS[COUNT_OBJECTS] = {
    {SYM_OBJ_ERR,       COUNT_OBJ_INF,      false,  "images/Texture/TextureError.png",  0,   0, {}},
    {SYM_OBJ_WALL,      COUNT_OBJ_INF,      false,  "images/Texture/TextureWall.png",   20,  0, {}},
    {SYM_OBJ_ROAD,      COUNT_OBJ_INF,      true,   "images/Texture/TextureRoad.png",   5,   0, {}},
    {SYM_OBJ_BORDER,    COUNT_OBJ_INF,      false,  "images/Texture/TextureBorder.png", 100, 0, {}},
    {SYM_OBJ_PLAYER,    COUNT_OBJ_PLAYER,   true,   "images/Texture/TexturePlayer.png", 0,   0, {}},
    {SYM_OBJ_COIN,      COUNT_OBJ_COIN,     true,   "images/Texture/TextureCoin.png",   10,  0, {}},
    {SYM_OBJ_LAMP,      COUNT_OBJ_LAMP,     false,  "images/Texture/TextureLamp.png",   0,   2, {SYM_OBJ_WALL, SYM_OBJ_BORDER}}
};

static void lab_gen    (char* lab);
static void lab_step   (char* lab);
static void lab_fill_empty  (char* lab, PlayerSet_t* PlayerSet);
static void set_lighting    (Map_t* map);
static void set_light_lamp  (Map_t* map, int pos);
static void lab_write2file  (char* lab, FILE* f);
static void count_free_pos  (char* lab, int* count_free, int* frees_ind);
static void select_free_pos (char* lab, char* free_pos, int count_free, int* frees_ind);
static bool check_neighbors (Object* src_obj, char* lab, int pos);
static void set_free_pos    (char* lab, PlayerSet_t* PlayerSet);

void lab_create(Map_t* map, PlayerSet_t* PlayerSet)
{
    lab_gen(map->lab);

    int x = 0;
    while (x++ < STEPS_GEN)
        lab_step(map->lab);

    lab_fill_empty(map->lab, PlayerSet);

    set_lighting(map);

    FILE *f = fopen("lab.txt", "w");
    lab_write2file(map->lab, f);
}

static void set_lighting(Map_t* map)
{
    memset(map->light, 20, N * M);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (OBJECTS[k].symbol == map->lab[i * M + j] &&
                    OBJECTS[k].symbol == SYM_OBJ_LAMP) {
                    set_light_lamp(map, i * M + j);
                }
            }
        }
    }
}

static void set_light_lamp(Map_t* map, int pos)
{
    for (int dy = -light_dist; dy <= light_dist; dy++) {
        for (int dx = -light_dist; dx <= light_dist; dx++) {
            if (((pos + dy * M + dx) < 0) || ((pos + dy * M + dx) > N * M -1))
                continue;

            float dist = sqrtf((float)(dx * dx + dy * dy));

            unsigned char was_light = map->light[pos + dy * M + dx];
            unsigned char new_light = (unsigned char) (255 * (float)(1.f - MIN(1.f, dist / (float)light_dist / light_force)));
            unsigned char new_light_norm = MAX(0, MIN(255, new_light));
            map->light[pos + dy * M + dx] = (unsigned char) MIN(255, was_light + new_light_norm);
        }
    }
}

static void lab_gen(char* lab)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (byte_board(i, j))
                lab[i * M + j] = SYM_OBJ_BORDER;

            int x = (rand() % 100 + 1);
            if (x >= CHANCE_LIFE)
                lab[i * M + j] = SYM_OBJ_WALL;
            else
                lab[i * M + j] = SYM_OBJ_ROAD;
        }
    }
}

static void lab_step(char* lab)
{
    char new_m[N * M];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (byte_board(i, j)) {
                new_m[i * M + j] = SYM_OBJ_BORDER;
                continue;
            }
            int byte_val_neighbours = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (!byte_valid(dx, dy))
                        continue;
                    if (lab[(i + dx) * M + (j + dy)] == SYM_OBJ_WALL)
                        byte_val_neighbours++;
                }
            }
            if (byte_val_neighbours >= NEW_LIFE)
                new_m[i * M + j] = SYM_OBJ_WALL;
            else if (lab[i * M + j] == SYM_OBJ_WALL && byte_val_neighbours >= STAY_IN_LIFE)
                new_m[i * M + j] = SYM_OBJ_WALL;
            else 
                new_m[i * M + j] = SYM_OBJ_ROAD;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            lab[i * M + j] = new_m[i * M + j];
        }
    }
}

static void lab_fill_empty(char* lab, PlayerSet_t* PlayerSet)
{
    int count_free = 0;
    int* frees_ind = (int*) calloc(N * M,  sizeof(int));
    count_free_pos(lab, &count_free, frees_ind);
    
    char* free_pos = (char*) calloc((size_t)count_free, sizeof(char));
    select_free_pos(lab, free_pos, count_free, frees_ind);
    set_free_pos(lab, PlayerSet);

    free(free_pos);
    free(frees_ind);
}

static void count_free_pos(char* lab, int* count_free, int* frees_ind)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (lab[i * M + j] == OBJECTS[k].symbol && OBJECTS[k].can_go) {
                    frees_ind[*count_free] = i * M + j;
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
            for (int ind = 0; ind < count_free; ind++) {
                int real_pos = (ind + pos) % count_free;
                if (!free_pos[real_pos] && check_neighbors(&OBJECTS[i], lab, frees_ind[real_pos])) {
                    OBJECTS[i].pos_num_free[j] = real_pos;
                    free_pos[real_pos] = 1;
                    break;
                }
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
                    if (OBJECTS[j].symbol == lab[pos + dy * M + dx] &&
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
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            bool skip = false;
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (lab[i * M + j] == OBJECTS[k].symbol && !OBJECTS[k].can_go) {
                    skip = true;
                    break;
                }
            }
            if (skip) continue;

            for (int k = 0; k < COUNT_OBJECTS; k++) {
                bool was = false;
                for (int m = 0; m < OBJECTS[k].count; m++) {
                    if (count_free == OBJECTS[k].pos_num_free[m]) {
                        lab[i * M + j] = OBJECTS[k].symbol;

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
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            if (lab[i * M + j] == ' ') count_free++;
    }
    int number_i = rand() % count_free;
    int number_o = rand() % count_free;
    while (number_o == number_i)
         number_o = rand() % count_free;

    count_free = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            fprintf(f, "%c", lab[i * M + j]);
        fprintf(f, "\n");
    }
}