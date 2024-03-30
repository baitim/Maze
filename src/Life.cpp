#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "Life.h"

void lab_gen    (char* lab);
void lab_step   (char* lab);
void lab_print  (char* lab);
void lab_fill_empty(char* lab, XYset_t* XYset);
void lab_write2file(char* lab, FILE* f);

void lab_gen(char* lab)
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

void lab_step(char* lab)
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

void lab_fill_empty(char* lab, XYset_t* XYset)
{
    int count_free = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            if (lab[i * M + j] == SYM_OBJ_ROAD) count_free++;
    
    int number_i = rand() % count_free;
    int number_o = rand() % count_free;
    while (number_o == number_i)
         number_o = rand() % count_free;

    count_free = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (count_free == number_i) { lab[i * M + j] = SYM_OBJ_PLAYER; XYset->px = j; XYset->py = i; }
            else if (count_free == number_o) lab[i * M + j] = SYM_OBJ_COIN;
            if (lab[i * M + j] == SYM_OBJ_ROAD   || 
                lab[i * M + j] == SYM_OBJ_PLAYER || 
                lab[i * M + j] == SYM_OBJ_COIN) 
                count_free++;
        }
    }
}

void lab_create(char* lab, XYset_t* XYset)
{
    lab_gen(lab);

    int x = 0;
    while (x++ < STEPS_GEN)
        lab_step(lab);

    lab_fill_empty(lab, XYset);

    FILE *f = fopen("lab.txt", "w");
    lab_write2file(lab, f);
}

void lab_print(char* lab)
{
    printf("\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            printf("%c", lab[i * M + j]);
        printf("\n");
    }
    printf("\n");
}

void lab_write2file(char* lab, FILE* f) 
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