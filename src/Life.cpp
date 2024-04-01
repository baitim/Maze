#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "Life.h"

Object OBJECTS[COUNT_OBJECTS] = {
    {SYM_OBJ_WALL,      COUNT_OBJ_INF,      false,  "images/Texture/TextureWall.png",   {}},
    {SYM_OBJ_ROAD,      COUNT_OBJ_INF,      true,   "images/Texture/TextureRoad.png",   {}},
    {SYM_OBJ_BORDER,    COUNT_OBJ_INF,      false,  "images/Texture/TextureBorder.png", {}},
    {SYM_OBJ_PLAYER,    COUNT_OBJ_PLAYER,   true,   "images/Texture/TexturePlayer.png", {}},
    {SYM_OBJ_COIN,      COUNT_OBJ_COIN,     true,   "images/Texture/TextureCoin.png",   {}}
};

static void lab_gen    (char* lab);
static void lab_step   (char* lab);
static void lab_print  (char* lab);
static void lab_fill_empty  (char* lab, PlayerSet_t* PlayerSet);
static void lab_write2file  (char* lab, FILE* f);
static void count_free_pos  (char* free_pos, int* count_free);
static void select_free_pos (char* free_pos, int count_free);
static void set_free_pos    (char* lab, char* free_pos, PlayerSet_t* PlayerSet);

void lab_create(char* lab, PlayerSet_t* PlayerSet)
{
    lab_gen(lab);

    int x = 0;
    while (x++ < STEPS_GEN)
        lab_step(lab);

    lab_fill_empty(lab, PlayerSet);

    FILE *f = fopen("lab.txt", "w");
    lab_write2file(lab, f);
}

static void lab_gen(char* lab)
{
    srand(time(NULL));
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
                    if (!byte_valID(dx, dy))
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
    count_free_pos(lab, &count_free);
    
    char* free_pos = (char*) malloc(count_free * sizeof(char));
    memset(free_pos, 0, count_free);

    select_free_pos(free_pos, count_free);
    set_free_pos(lab, free_pos, PlayerSet);

    free(free_pos);
}

static void count_free_pos(char* lab, int* count_free)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            for (int k = 0; k < COUNT_OBJECTS; k++)
                if (lab[i * M + j] == OBJECTS[k].symbol && OBJECTS[k].can_go) 
                    (*count_free)++;
}

static void select_free_pos(char* free_pos, int count_free)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (OBJECTS[i].count == COUNT_OBJ_INF) 
            continue;

        OBJECTS[i].pos_num_free = (int*) malloc(OBJECTS[i].count * sizeof(int));
        for (int j = 0; j < OBJECTS[i].count; j++) {
            int pos = rand() % count_free;
            for (int ind = 0; ind < count_free; ind++) {
                if (free_pos[(ind + pos) % count_free]) continue;
                else {
                    OBJECTS[i].pos_num_free[j] = (ind + pos) % count_free;
                    free_pos[(ind + pos) % count_free] = 1;
                    break;
                }
            }
        }
    }
}

static void set_free_pos(char* lab, char* free_pos, PlayerSet_t* PlayerSet)
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

static void lab_print(char* lab)
{
    printf("\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            printf("%c", lab[i * M + j]);
        printf("\n");
    }
    printf("\n");
}