#ifndef SRC_CONFIG_H
#define SRC_CONFIG_H

#define hbyte2pix (int)4
#define wbyte2pix (int)4
#define PIX_HEIGHT (int)1000
#define PIX_WIDTH  (int)1200
#define BYTE_HEIGHT (int)(PIX_HEIGHT / hbyte2pix)
#define BYTE_WIDTH  (int)(PIX_WIDTH  / wbyte2pix)
#define SCREEN_BYTES_COUNT (int)(BYTE_HEIGHT * BYTE_WIDTH)
#define STEPS_GEN (int)18
#define CHANCE_LIFE  (double)52.f
#define STAY_IN_LIFE (int)4
#define NEW_LIFE (int)5
#define hscale_render (int)4
#define wscale_render (int)4
#define delay_dx_max (int)12
#define delay_dy_max (int)12
#define delay_path_count (int)9
#define delay_info_count (int)150
#define light_dist (int)20
#define light_force (double)1.f
#define MAX_SIZE_INFO_STR (int)100
#define RENDER_THREADS (int)3
#define TUNNELS_WIDTH  (int)2
#define HILL_SLOPE (int)40
#define MAX_HILL_HEIGHT (int)5
#define AUDIO_FREQUENCY 44100

typedef struct Path_t_ {
    int path[SCREEN_BYTES_COUNT];
    int count;
    int passed;
    int path_target;
    int path_exist;
} Path_t;

typedef struct Map_t_ {
    char map[SCREEN_BYTES_COUNT];
    unsigned char col[SCREEN_BYTES_COUNT * 3];
    unsigned char light[SCREEN_BYTES_COUNT];
    unsigned char shadow[SCREEN_BYTES_COUNT];
    Path_t path;
} Map_t;

typedef enum ObjectsSymbols_ {
    SYM_OBJ_WALL    = '#',
    SYM_OBJ_ROAD    = ' ',
    SYM_OBJ_BORDER  = '*',
    SYM_OBJ_PLAYER  = 'P',
    SYM_OBJ_COIN    = 'C',
    SYM_OBJ_LAMP    = 'L',
    SYM_OBJ_PATH    = '`',
    SYM_OBJ_DEST    = 'V',
    SYM_OBJ_IMDEST  = 'X',
    SYM_OBJ_TUNNEL  = 'T',
    SYM_OBJ_ERR     = '!',
    SYM_OBJ_SCREAM  = 'S'
} ObjectsSymbols;

typedef enum CountObject2Render_ {
    COUNT_OBJ_INF =    -1,
    COUNT_OBJ_PLAYER =  1,
    COUNT_OBJ_COIN =   50,
    COUNT_OBJ_LAMP =   80,
    COUNT_OBJ_PATH =   -2,
    COUNT_OBJ_SCREAM = 25
} CountObject2Render;

#define COUNT_OBJECTS 12

typedef struct Object_t_ {
    ObjectsSymbols symbol;
    CountObject2Render count;
    int can_go;
    const char* name_src_file;
    const int transparency;
    const int count_neighbors;
    ObjectsSymbols neighbors[COUNT_OBJECTS];
    unsigned char bytes_color[hbyte2pix * wbyte2pix * 4];
} Object_t;
extern Object_t OBJECTS[COUNT_OBJECTS];

#endif // SRC_CONFIG_H