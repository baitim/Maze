#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define BOARD(x, y) ((x) == 0 || (y) == 0 || (x) == N - 1 || (y) == M - 1)
#define VALID(dx, dy) (((dx) != 0) || ((dy) != 0))

const int steps = 15;
const int N = 40;
const int M = 160;
const double chance = 52;
const int stay_in_life = 4;
const int new_life = 5;
char val = '*';
char inv = ' ';
char board = '#';

void lab_gen(char *lab);
void lab_step(char *lab);
void lab_print(char *lab);

void lab_gen(char *lab)
{
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (BOARD(i, j))
                lab[i * M + j] = board;
            int x = (rand() % 100 + 1);
            if (x >= chance)
                lab[i * M + j] = val;
            else
                lab[i * M + j] = inv;
        }
    }
}

void lab_step(char *lab)
{
    char new_m[N * M];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (BOARD(i, j)) {
                new_m[i * M + j] = board;
                continue;
            }
            int val_neighbours = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (!VALID(dx, dy))
                        continue;
                    if (lab[(i + dx) * M + (j + dy)] == val)
                        val_neighbours++;
                }
            }
            if (val_neighbours >= new_life)
                new_m[i * M + j] = val;
            else if (lab[i * M + j] == val && val_neighbours >= stay_in_life)
                new_m[i * M + j] = val;
            else 
                new_m[i * M + j] = inv;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            lab[i * M + j] = new_m[i * M + j];
        }
    }
}

void lab_print(char *lab)
{
    printf("\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            printf("%c", lab[i * M + j]);
        printf("\n");
    }
    printf("\n");
}

void write_lab(char *lab, FILE *f) 
{
    srand(time(NULL));
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
        for (int j = 0; j < M; j++) {
            if (count_free == number_i) fprintf(f, "i");
            else if (count_free == number_o) fprintf(f, "o");
            else if (lab[i * M + j] == val || lab[i * M + j] == board) fprintf(f, "#");
            else fprintf(f, " ");
            if (lab[i * M + j] == ' ') count_free++;
        }
        fprintf(f, "\n");
    }
}

int main()
{
    char lab[N * M];
    lab_gen(lab);

    int x = 0;
    while (x < steps) {
        lab_step(lab);
        x++;
    }
    
    lab_print(lab);

    FILE *f = fopen("lab.txt", "w");
    if (!f) return -fprintf(stderr, "Cannot open file\n");

    write_lab(lab, f);

    return 0;
}