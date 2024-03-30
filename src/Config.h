#ifndef CONFIG_H
#define CONFIG_H

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define byte_board(x, y) ((x) == 0 || (y) == 0 || (x) == N - 1 || (y) == M - 1)
#define byte_valID(dx, dy) (((dx) != 0) || ((dy) != 0))
#define POS(x, y) ((y) * WIDTH + x) * 4

const int hbyte2pix = 4;
const int wbyte2pix = 4;
const int HEIGHT = 1000;
const int WIDTH  = 1200;
const int N = HEIGHT / hbyte2pix;
const int M = WIDTH  / wbyte2pix;
const int STEPS_GEN = 16;
const double CHANCE_LIFE = 52;
const int STAY_IN_LIFE = 4;
const int NEW_LIFE = 5;
const int hscale_render = 4;
const int wscale_render = 4;

enum SYMBOLS_OBJECTS {
    SYM_OBJ_WALL    = '#',
    SYM_OBJ_ROAD    = ' ',
    SYM_OBJ_BORDER  = '*',
    SYM_OBJ_PLAYER  = 'P',
    SYM_OBJ_COIN    = 'C',
};

struct Object {
    SYMBOLS_OBJECTS symbol;
    bool can_go;
    const char* name_src_file;
    char bytes_color[hbyte2pix * wbyte2pix * 4];
};

const int COUNT_OBJECTS = 5;
extern Object OBJECTS[COUNT_OBJECTS];

#endif // CONFIG_H