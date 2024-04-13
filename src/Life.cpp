#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Life.h"
#include "Math.h"

typedef struct Tunnel_t_ {
    int point_start;
    int point_finish;
} Tunnel_t;

typedef struct Point_t_ {
    int x;
    int y;
    int ind;
} Point_t;

typedef struct MapRoom_t_ {
    int count_rooms;
    int* obj_count;
    Point_t** obj_ind;
} MapRoom_t;

Object OBJECTS[COUNT_OBJECTS] = {
    {SYM_OBJ_WALL,      COUNT_OBJ_INF,      false,  "images/Texture/TextureWall.png",   20,  0, {}},
    {SYM_OBJ_ROAD,      COUNT_OBJ_INF,      true,   "images/Texture/TextureRoad.png",   5,   0, {}},
    {SYM_OBJ_BORDER,    COUNT_OBJ_INF,      false,  "images/Texture/TextureBorder.png", 100, 0, {}},
    {SYM_OBJ_PLAYER,    COUNT_OBJ_PLAYER,   true,   "images/Texture/TexturePlayer.png", 0,   0, {}},
    {SYM_OBJ_COIN,      COUNT_OBJ_COIN,     true,   "images/Texture/TextureCoin.png",   10,  0, {}},
    {SYM_OBJ_LAMP,      COUNT_OBJ_LAMP,     false,  "images/Texture/TextureLamp.png",   0,   2, {SYM_OBJ_WALL, SYM_OBJ_BORDER}},
    {SYM_OBJ_PATH,      COUNT_OBJ_PATH,     true,   "images/Texture/TexturePath.png",   0,   0, {}},
    {SYM_OBJ_DEST,      COUNT_OBJ_PATH,     true,   "images/Texture/TextureDest.png",   0,   0, {}},
    {SYM_OBJ_IMDEST,    COUNT_OBJ_PATH,     true,   "images/Texture/TextureImDest.png", 0,   0, {}},
    {SYM_OBJ_TUNNEL,    COUNT_OBJ_INF,      true,   "images/Texture/TextureRoad.png",   5,   0, {}},
    {SYM_OBJ_ERR,       COUNT_OBJ_INF,      false,  "images/Texture/TextureError.png",  0,   0, {}}
};

static void map_create_connectivity(char* map);
static void set_room_connection    (char* map, Tunnel_t* tunnels, int count, int* mst);
static void draw_line       (char* map, int x1, int y1, int x2, int y2);
static void get_graph_mst   (int* graph, int count, int* mst);
static void make_graph      (MapRoom_t* map_room, int* graph, Tunnel_t* tunnels);
static void paint_graph     (MapRoom_t* map_room, char* map);
static int  paint_subgraph  (int v, int color, int* colors, char* map);
static void map_room_delete (MapRoom_t* map_room);
static void map_gen         (char* map);
static void map_step        (char* map);
static void map_fill_empty  (char* map, PlayerSet_t* PlayerSet);
static void set_lighting    (Map_t* map);
static void set_light_lamp  (Map_t* map, int x, int y);
static void map_write2file  (char* map, FILE* f);
static void count_free_pos  (char* map, int* count_free, int* frees_ind);
static void select_free_pos (char* map, char* free_pos, int count_free, int* frees_ind);
static bool check_neighbors (Object* src_obj, char* map, int pos);
static void set_free_pos    (char* map, PlayerSet_t* PlayerSet);
static int  is_obj_on_border(int x, int y);
static int  obj_can_set     (char* map, int map_ind);
static int  get_obj_index   (int obj_sym);

int pos_in_pix_window(int x, int y)
{
    return (y * PIX_WIDTH + x) * 4;
}

void map_create(Map_t* map, PlayerSet_t* PlayerSet, char* output_file)
{
    memset(map->map,   0, sizeof(char) * BYTE_HEIGHT * BYTE_WIDTH);
    memset(map->light, 0, sizeof(unsigned char) * BYTE_HEIGHT * BYTE_WIDTH);
    memset(map->col,   0, sizeof(unsigned char) * BYTE_HEIGHT * BYTE_WIDTH * 3);
    
    map_gen(map->map);

    int x = 0;
    while (x++ < STEPS_GEN)
        map_step(map->map);

    map_create_connectivity(map->map);

    map_fill_empty(map->map, PlayerSet);

    set_lighting(map);

    FILE *f = fopen(output_file, "w");
    map_write2file(map->map, f);
}

static void map_create_connectivity(char* map)
{
    MapRoom_t map_room = {};
    paint_graph(&map_room, map);

    Tunnel_t* tunnels = (Tunnel_t*) calloc(sizeof(Tunnel_t), (size_t)(map_room.count_rooms * map_room.count_rooms));
    int* graph = (int*) calloc(sizeof(int), (size_t)(map_room.count_rooms * map_room.count_rooms));

    for (int i = 0; i < map_room.count_rooms * map_room.count_rooms; i++) {
        tunnels[i].point_start  = -1;
        tunnels[i].point_finish = -1;
        graph[i] = -1;
    }

    make_graph(&map_room, graph, tunnels);

    int* mst = (int*) calloc((size_t)(map_room.count_rooms), sizeof(int));
    get_graph_mst(graph, map_room.count_rooms, mst);

    set_room_connection(map, tunnels, map_room.count_rooms, mst);

    free(graph);
    free(tunnels);
    free(mst);
    map_room_delete(&map_room);
}

static void set_room_connection(char* map, Tunnel_t* tunnels, int count, int* mst)
{
    for (int i = 1; i < count; i++) {
        int start  = tunnels[mst[i] * count + i].point_start;
        int finish = tunnels[mst[i] * count + i].point_finish;
        
        int x1 = start  % BYTE_WIDTH;
        int y1 = start  / BYTE_WIDTH;
        int x2 = finish % BYTE_WIDTH;
        int y2 = finish / BYTE_WIDTH;

        draw_line(map, x1, y1, x2, y2);
    }
}

static void draw_line(char* map, int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int e2;
    
    int x = x1, y = y1;
    while (!(x == x2 && y == y2)) {
        map[y * BYTE_WIDTH + x] = SYM_OBJ_TUNNEL;
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }

        map[y * BYTE_WIDTH + x] = SYM_OBJ_TUNNEL;
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void get_graph_mst(int* graph, int count, int* mst)
{
    int* weight = (int*) calloc(sizeof(int), (size_t)count);
    int* visited = (int*) calloc(sizeof(int), (size_t)count);
    for (int i = 0; i < count; i++) {
        weight[i] = BYTE_HEIGHT * BYTE_WIDTH;
        visited[i] = 0;
    }

    weight[0] = 0;
    mst[0] = -1;

    int u;
    for (int i = 0; i < count; i++) {
        
        int min_weight = BYTE_HEIGHT * BYTE_WIDTH;
        for (int v = 0; v < count; v++) {
            if (visited[v] == 0 && weight[v] < min_weight) {
                min_weight = weight[v];
                u = v;
            }
        }

        visited[u] = 1;

        for (int v = 0; v < count; v++) {
            int ind = u * count + v;

            if (graph[ind] != -1 && visited[v] == 0 && graph[ind] < weight[v]) {
                mst[v] = u;
                weight[v] = graph[ind];
            }
        }
    }

    free(visited);
    free(weight);
}

static void make_graph(MapRoom_t* map_room, int* graph, Tunnel_t* tunnels)
{
    for (int i = 0; i < map_room->count_rooms; i++) {
        for (int j = i + 1; j < map_room->count_rooms; j++) {
            int min_dist = BYTE_HEIGHT * BYTE_WIDTH;
            int start_min, finish_min;
            for (int a = 0; a < map_room->obj_count[i]; a++) {
                int x1 = map_room->obj_ind[i][a].x;
                int y1 = map_room->obj_ind[i][a].y;
                for (int b = 0; b < map_room->obj_count[j]; b++) {
                    int x2 = map_room->obj_ind[j][b].x;
                    int y2 = map_room->obj_ind[j][b].y;
                    int dist = abs(y2 - y1) + abs(x2 - x1);

                    if (min_dist > dist) {
                        min_dist = dist;
                        start_min  = map_room->obj_ind[i][a].ind;
                        finish_min = map_room->obj_ind[j][b].ind;
                    }
                }
            }

            graph[i * map_room->count_rooms + j] = min_dist;
            graph[j * map_room->count_rooms + i] = min_dist;
            tunnels[i * map_room->count_rooms + j].point_start  = start_min;
            tunnels[i * map_room->count_rooms + j].point_finish = finish_min;
            tunnels[j * map_room->count_rooms + i].point_start  = finish_min;
            tunnels[j * map_room->count_rooms + i].point_finish = start_min;
        }
    }
}

static void paint_graph(MapRoom_t* map_room, char* map)
{
    int* colors = (int*) calloc(sizeof(int), (size_t)(BYTE_HEIGHT * BYTE_WIDTH));
    int* childs = (int*) calloc(sizeof(int), (size_t)(BYTE_HEIGHT * BYTE_WIDTH));
    
    int color = 1;
    for (int i = 0; i < BYTE_HEIGHT * BYTE_WIDTH; i++) {
        if (colors[i] == 0 && obj_can_set(map, i)) {
            childs[color - 1] += paint_subgraph(i, color, colors, map);
            color++;
        }
    }
    int count_rooms = color - 1;

    map_room->count_rooms = count_rooms;
    map_room->obj_count = (int*)  calloc(sizeof(int),  (size_t)(count_rooms));
    map_room->obj_ind   = (Point_t**) calloc(sizeof(Point_t*), (size_t)(count_rooms));

    for (int i = 0; i < count_rooms; i++) 
        map_room->obj_ind[i] = (Point_t*) calloc(sizeof(Point_t), (size_t)(childs[i]));

    for (int i = 0; i < BYTE_HEIGHT * BYTE_WIDTH; i++) {
        if (colors[i] > 0) {
            int ind = map_room->obj_count[colors[i] - 1];

            map_room->obj_ind[colors[i] - 1][ind].x = i % BYTE_WIDTH;
            map_room->obj_ind[colors[i] - 1][ind].y = i / BYTE_WIDTH;
            map_room->obj_ind[colors[i] - 1][ind].ind = i;

            map_room->obj_count[colors[i] - 1]++;
        }
    }
    
    free(childs);
    free(colors);
}

static int paint_subgraph(int v, int color, int* colors, char* map)
{
    int count_childs = 0;
    colors[v] = color;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (abs(dx) + abs(dy) != 1)
                continue;

            int pos = v + dy * BYTE_WIDTH + dx;
            if (pos >= BYTE_HEIGHT * BYTE_WIDTH || pos < 0)
                continue;

            if (colors[pos] == 0 && obj_can_set(map, pos))
                count_childs += paint_subgraph(pos, color, colors, map);
        }
    }
    return count_childs + 1;
}

static void map_room_delete(MapRoom_t* map_room)
{
    for (int i = 0; i < map_room->count_rooms; i++)
        free(map_room->obj_ind[i]);
    free(map_room->obj_ind);
    free(map_room->obj_count);
}

static void set_lighting(Map_t* map)
{
    int ind_obj_lamp = -1;
    for (int i = 0; i < COUNT_OBJECTS; i++)
        if (OBJECTS[i].symbol == SYM_OBJ_LAMP)
            ind_obj_lamp = i;

    memset(map->light, 25, BYTE_HEIGHT * BYTE_WIDTH);
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            if (OBJECTS[ind_obj_lamp].symbol == map->map[i * BYTE_WIDTH + j]) {
                set_light_lamp(map, j, i);
            }
        }
    }
}

static void set_light_lamp(Map_t* map, int x, int y)
{
    int pos = y * BYTE_WIDTH + x;
    for (int dy = -light_dist; dy <= light_dist; dy++) {
        for (int dx = -light_dist; dx <= light_dist; dx++) {
            int dpos = pos + dy * BYTE_WIDTH + dx;

            if ((dpos < 0) || (dpos > BYTE_HEIGHT * BYTE_WIDTH -1))
                continue;

            if (dx + x >= BYTE_WIDTH || dx + x < 0)
                continue;

            float dist = sqrtf((float)(dx * dx + dy * dy));
            unsigned char ratio = 255 * (float)(1.f - MIN(1.f, dist / (float)light_dist / light_force));
            unsigned char ratio_norm = MAX(0, MIN(255, ratio));

            unsigned char was_light = map->light[pos + dy * BYTE_WIDTH + dx];
            map->light[pos + dy * BYTE_WIDTH + dx] = (unsigned char) MIN(255, was_light + ratio_norm);

            unsigned char was_red   = map->col[dpos * 3 + 0];
            unsigned char was_green = map->col[dpos * 3 + 1];
            map->col[dpos * 3 + 0] = MIN(255, was_red   + ratio_norm / 7);
            map->col[dpos * 3 + 1] = MIN(255, was_green + ratio_norm / 7);
        }
    }
}

static void map_gen(char* map)
{
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            int pos = i * BYTE_WIDTH + j;
            if (is_obj_on_border(i, j))
                map[pos] = SYM_OBJ_BORDER;

            int x = (rand() % 100 + 1);
            if (x >= CHANCE_LIFE)
                map[pos] = SYM_OBJ_WALL;
            else
                map[pos] = SYM_OBJ_ROAD;
        }
    }
}

static void map_step(char* map)
{
    char new_m[BYTE_HEIGHT * BYTE_WIDTH];
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            int pos = i * BYTE_WIDTH + j;
            if (is_obj_on_border(i, j)) {
                new_m[pos] = SYM_OBJ_BORDER;
                continue;
            }
            int byte_val_neighbours = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (!(((dx) != 0) || ((dy) != 0)))
                        continue;
                    if (map[(i + dx) * BYTE_WIDTH + (j + dy)] == SYM_OBJ_WALL)
                        byte_val_neighbours++;
                }
            }
            if (byte_val_neighbours >= NEW_LIFE)
                new_m[pos] = SYM_OBJ_WALL;
            else if (map[pos] == SYM_OBJ_WALL && byte_val_neighbours >= STAY_IN_LIFE)
                new_m[pos] = SYM_OBJ_WALL;
            else 
                new_m[pos] = SYM_OBJ_ROAD;
        }
    }
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            int pos = i * BYTE_WIDTH + j;
            map[pos] = new_m[pos];
        }
    }
}

static void map_fill_empty(char* map, PlayerSet_t* PlayerSet)
{
    int count_free = 0;
    int* frees_ind = (int*) calloc(BYTE_HEIGHT * BYTE_WIDTH,  sizeof(int));
    count_free_pos(map, &count_free, frees_ind);
    
    char* free_pos = (char*) calloc((size_t)count_free, sizeof(char));
    select_free_pos(map, free_pos, count_free, frees_ind);
    set_free_pos(map, PlayerSet);

    free(free_pos);
    free(frees_ind);
}

static void count_free_pos(char* map, int* count_free, int* frees_ind)
{
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            int pos = i * BYTE_WIDTH + j;
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (map[pos] == OBJECTS[k].symbol && OBJECTS[k].can_go) {
                    frees_ind[*count_free] = pos;
                    (*count_free)++;
                }
            }
        }
    }
}

static void select_free_pos(char* map, char* free_pos, int count_free, int* frees_ind)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (OBJECTS[i].count <= 0) 
            continue;

        OBJECTS[i].pos_num_free = (int*) malloc((size_t)OBJECTS[i].count * sizeof(int));
        for (int j = 0; j < OBJECTS[i].count; j++) {
            int pos = rand() % count_free;

            while (1) {
                if (!free_pos[pos] && check_neighbors(&OBJECTS[i], map, frees_ind[pos])) {
                    OBJECTS[i].pos_num_free[j] = pos;
                    free_pos[pos] = 1;
                    break;
                }
                pos = rand() % count_free;
            }
        }
    }
}

static bool check_neighbors(Object* src_obj, char* map, int pos)
{
    if (src_obj->count_neighbors == 0) 
        return true;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (abs(dx) + abs(dy) != 1)
                continue;
            
            for (int i = 0; i < src_obj->count_neighbors; i++) {
                for (int j = 0; j < COUNT_OBJECTS; j++) {
                    if (OBJECTS[j].symbol == map[pos + dy * BYTE_WIDTH + dx] &&
                        OBJECTS[j].symbol == src_obj->neighbors[i])
                        return true;
                }
            }
        }
    }
    return false;
}

static void set_free_pos(char* map, PlayerSet_t* PlayerSet)
{
    int count_free = 0;
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++) {
            bool skip = false;
            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (map[i * BYTE_WIDTH + j] == OBJECTS[k].symbol && !OBJECTS[k].can_go) {
                    skip = true;
                    break;
                }
            }
            if (skip) continue;

            for (int k = 0; k < COUNT_OBJECTS; k++) {
                if (OBJECTS[k].count <= 0) 
                    continue;

                bool was = false;
                for (int m = 0; m < OBJECTS[k].count; m++) {
                    if (count_free == OBJECTS[k].pos_num_free[m]) {
                        map[i * BYTE_WIDTH + j] = OBJECTS[k].symbol;

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

static void map_write2file(char* map, FILE* f) 
{
    int count_free = 0;
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++)
            if (map[i * BYTE_WIDTH + j] == ' ') count_free++;
    }
    int number_i = rand() % count_free;
    int number_o = rand() % count_free;
    while (number_o == number_i)
         number_o = rand() % count_free;

    count_free = 0;
    for (int i = 0; i < BYTE_HEIGHT; i++) {
        for (int j = 0; j < BYTE_WIDTH; j++)
            fprintf(f, "%c", map[i * BYTE_WIDTH + j]);
        fprintf(f, "\n");
    }
}

static int is_obj_on_border(int x, int y)
{
    return (x == 0 || y == 0 || x == BYTE_HEIGHT - 1 || y == BYTE_WIDTH - 1);
}

static int obj_can_set(char* map, int map_ind)
{
    for (int k = 0; k < COUNT_OBJECTS; k++)
        if (map[map_ind] == OBJECTS[k].symbol && !OBJECTS[k].can_go)
            return 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            
            int pos = map_ind + dy * BYTE_WIDTH + dx;
            if (pos < 0 || pos >= BYTE_HEIGHT * BYTE_WIDTH)
                continue;

            if (map[pos] == SYM_OBJ_TUNNEL)
                return 0;
        }
    }
    return 1;
}

static int get_obj_index(int obj_sym)
{
    for (int i = 0; i < COUNT_OBJECTS; i++)
        if (OBJECTS[i].symbol == obj_sym)
            return i;
    return -1;
}