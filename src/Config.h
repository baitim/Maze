#ifndef SRC_CONFIG_H
#define SRC_CONFIG_H

#define byte_board(x, y) ((x) == 0 || (y) == 0 || (x) == BYTE_HEIGHT - 1 || (y) == BYTE_WIDTH - 1)
#define POS(x, y) ((y) * PIX_WIDTH + x) * 4

const int hbyte2pix = 4;
const int wbyte2pix = 4;
const int PIX_HEIGHT = 1000;
const int PIX_WIDTH  = 1200;
const int BYTE_HEIGHT = PIX_HEIGHT / hbyte2pix;
const int BYTE_WIDTH = PIX_WIDTH  / wbyte2pix;
const int STEPS_GEN = 16;
const double CHANCE_LIFE = 52;
const int STAY_IN_LIFE = 4;
const int NEW_LIFE = 5;
const int hscale_render = 4;
const int wscale_render = 4;
const int delay_dx_max = 7;
const int delay_dy_max = 7;
const int delay_dy_path = 5;
const int light_dist = 20;
const int light_force = 1.2;
const int MAX_SIZE_INFO_STR = 100;

struct Path_t {
    int path[BYTE_HEIGHT * BYTE_WIDTH];
    int count;
    int passed;
    int path_target;
    int path_exist;
};

struct Map_t {
    char lab[BYTE_HEIGHT * BYTE_WIDTH];
    unsigned char col[BYTE_HEIGHT * BYTE_WIDTH * 3];
    unsigned char light[BYTE_HEIGHT * BYTE_WIDTH];
    Path_t path;
};

enum SYMBOLS_OBJECTS {
    SYM_OBJ_ERR     = '!',
    SYM_OBJ_WALL    = '#',
    SYM_OBJ_ROAD    = ' ',
    SYM_OBJ_BORDER  = '*',
    SYM_OBJ_PLAYER  = 'P',
    SYM_OBJ_COIN    = 'C',
    SYM_OBJ_LAMP    = 'L',
    SYM_OBJ_PATH    = '`',
    SYM_OBJ_DEST    = 'V',
    SYM_OBJ_IMDEST  = 'X'
};

enum COUNT_OBJECTS_REN {
    COUNT_OBJ_INF = -1,
    COUNT_OBJ_PLAYER = 1,
    COUNT_OBJ_COIN = 50,
    COUNT_OBJ_LAMP = 70
};

const int COUNT_OBJECTS = 10;

struct Object {
    SYMBOLS_OBJECTS symbol;
    COUNT_OBJECTS_REN count;
    bool can_go;
    const char* name_src_file;
    const int transparency;
    const int count_neighbors;
    SYMBOLS_OBJECTS neighbors[COUNT_OBJECTS];
    unsigned char bytes_color[hbyte2pix * wbyte2pix * 4];
    int* pos_num_free;
};
extern Object OBJECTS[COUNT_OBJECTS];

#endif // SRC_CONFIG_H