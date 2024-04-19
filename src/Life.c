#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Life.h"
#include "Math.h"
#include "Queue.h"

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

Object_t OBJECTS[COUNT_OBJECTS] = {
    {SYM_OBJ_WALL,      COUNT_OBJ_INF,      0,  "images/Texture/TextureWall.png",   20,  0, {}},
    {SYM_OBJ_ROAD,      COUNT_OBJ_INF,      1,  "images/Texture/TextureRoad.png",   5,   0, {}},
    {SYM_OBJ_BORDER,    COUNT_OBJ_INF,      0,  "images/Texture/TextureBorder.png", 100, 0, {}},
    {SYM_OBJ_PLAYER,    COUNT_OBJ_PLAYER,   1,  "images/Texture/TexturePlayer.png", 0,   0, {}},
    {SYM_OBJ_COIN,      COUNT_OBJ_COIN,     1,  "images/Texture/TextureCoin.png",   10,  0, {}},
    {SYM_OBJ_LAMP,      COUNT_OBJ_LAMP,     0,  "images/Texture/TextureLamp.png",   0,   2, {SYM_OBJ_WALL}},
    {SYM_OBJ_PATH,      COUNT_OBJ_PATH,     1,  "images/Texture/TexturePath.png",   0,   0, {}},
    {SYM_OBJ_DEST,      COUNT_OBJ_PATH,     1,  "images/Texture/TextureDest.png",   0,   0, {}},
    {SYM_OBJ_IMDEST,    COUNT_OBJ_PATH,     1,  "images/Texture/TextureImDest.png", 0,   0, {}},
    {SYM_OBJ_TUNNEL,    COUNT_OBJ_INF,      1,  "images/Texture/TextureRoad.png",   5,   0, {}},
    {SYM_OBJ_ERR,       COUNT_OBJ_INF,      0,  "images/Texture/TextureError.png",  0,   0, {}}
};

static ErrorCode map_create_connectivity(char* map);
static ErrorCode set_room_connection    (char* map, Tunnel_t* tunnels, int count, int* mst);
static void      get_tunnel             (int* points, int x1, int y1, int x2, int y2);
static void      draw_circle            (char* map, int* points);
static ErrorCode get_graph_mst          (int* graph, int count, int* mst);
static void      make_graph             (MapRoom_t* map_room, int* graph, Tunnel_t* tunnels);
static ErrorCode paint_graph            (MapRoom_t* map_room, char* map);
static int       paint_subgraph         (int v, int color, int* colors, char* map);
static ErrorCode map_room_init          (MapRoom_t* map_room, int count_rooms, int* childs, int* colors);
static void      map_room_delete        (MapRoom_t* map_room);
static ErrorCode make_hills             (Map_t* map);
static ErrorCode step_get_height        (int* distance, char* map, int x, int y, Queue_t** queue, int* res_step);
static ErrorCode get_height             (char* map, int pos, int* height);
static void      map_gen                (char* map);
static void      map_step               (char* map);
static ErrorCode map_fill_empty         (char* map, PlayerSet_t* PlayerSet);
static void      set_lighting           (Map_t* map);
static void      set_light_lamp         (Map_t* map, int pos);
static void      map_write2file         (char* map, FILE* file);
static void      count_free_pos         (char* map, int* count_free, int* frees_ind);
static void      select_free_pos        (char* map, char* free_pos, int count_free, int* frees_ind, PlayerSet_t* PlayerSet);
static int       check_neighbors        (Object_t* src_obj, char* map, int pos);
static int       is_obj_on_border       (int pos);
static int       obj_can_set            (char* map, int map_ind);
static int       is_pos_in_byte_window  (int pos);

int pos_in_pix_window(int x, int y)
{
    return (y * PIX_WIDTH + x) * 4;
}

ErrorCode map_create(Map_t* map, PlayerSet_t* PlayerSet, char* output_file)
{
    ErrorCode error = ERROR_NO;
    memset(map->map,    0, sizeof(char) * SCREEN_BYTES_COUNT);
    memset(map->light,  0, sizeof(unsigned char) * SCREEN_BYTES_COUNT);
    memset(map->col,    0, sizeof(unsigned char) * SCREEN_BYTES_COUNT * 3);
    memset(map->shadow, 0, sizeof(unsigned char) * SCREEN_BYTES_COUNT);
    
    map_gen(map->map);

    int x = 0;
    while (x++ < STEPS_GEN)
        map_step(map->map);

    error = map_create_connectivity(map->map);
    if (error) return error;

    error = map_fill_empty(map->map, PlayerSet);
    if (error) return error;

    error = make_hills(map);
    if (error) return error;

    set_lighting(map);

    FILE *file = fopen(output_file, "w");
    if (!file) return ERROR_OPEN_FILE;

    map_write2file(map->map, file);

    return ERROR_NO;
}

static ErrorCode map_create_connectivity(char* map)
{
    ErrorCode error = ERROR_NO;
    MapRoom_t map_room = {};
    error = paint_graph(&map_room, map);
    if (error) return error;

    Tunnel_t* tunnels = (Tunnel_t*) calloc(sizeof(Tunnel_t), (size_t)(map_room.count_rooms * map_room.count_rooms));
    if (!tunnels) return ERROR_ALLOC_FAIL;

    int* graph = (int*) calloc(sizeof(int), (size_t)(map_room.count_rooms * map_room.count_rooms));
    if (!graph) return ERROR_ALLOC_FAIL;

    for (int i = 0; i < map_room.count_rooms * map_room.count_rooms; i++) {
        tunnels[i].point_start  = -1;
        tunnels[i].point_finish = -1;
        graph[i] = -1;
    }

    make_graph(&map_room, graph, tunnels);

    int* mst = (int*) calloc((size_t)(map_room.count_rooms), sizeof(int));
    if (!mst) return ERROR_ALLOC_FAIL;

    error = get_graph_mst(graph, map_room.count_rooms, mst);
    if (error) return error;

    error = set_room_connection(map, tunnels, map_room.count_rooms, mst);
    if (error) return error;

    free(graph);
    free(tunnels);
    free(mst);
    map_room_delete(&map_room);

    return ERROR_NO;
}

static ErrorCode set_room_connection(char* map, Tunnel_t* tunnels, int count, int* mst)
{
    for (int i = 1; i < count; i++) {
        int start  = tunnels[mst[i] * count + i].point_start;
        int finish = tunnels[mst[i] * count + i].point_finish;
        
        int x1 = start  % BYTE_WIDTH;
        int y1 = start  / BYTE_WIDTH;
        int x2 = finish % BYTE_WIDTH;
        int y2 = finish / BYTE_WIDTH;

        int* points = (int*) calloc(sizeof(int), SCREEN_BYTES_COUNT);
        if (!points) return ERROR_ALLOC_FAIL;

        get_tunnel(points, x1, y1, x2, y2);
        draw_circle(map, points);
        free(points);
    }
    return ERROR_NO;
}

static void get_tunnel(int* points, int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int e2;
    
    int x = x1, y = y1;
    points[y * BYTE_WIDTH + x] = 1;
    while (!(x == x2 && y == y2)) {
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }

        points[y * BYTE_WIDTH + x] = 1;
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
        points[y * BYTE_WIDTH + x] = 1;
    }
}

static void draw_circle(char* map, int* points)
{
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++) {
        if (points[pos] == 0) continue;

        for (int dy = -TUNNELS_WIDTH; dy <= TUNNELS_WIDTH; dy++) {
            for (int dx = -TUNNELS_WIDTH; dx <= TUNNELS_WIDTH; dx++) {
                int dpos = pos + dy * BYTE_WIDTH + dx;

                if (!is_pos_in_byte_window(pos))
                    continue;

                if (is_obj_on_border(dpos))
                    continue;

                if (dx * dx + dy * dy <= TUNNELS_WIDTH)
                    map[dpos] = SYM_OBJ_TUNNEL;
            }
        }
    }
}

static ErrorCode get_graph_mst(int* graph, int count, int* mst)
{
    int* weight = (int*) calloc(sizeof(int), (size_t)count);
    if (!weight) return ERROR_ALLOC_FAIL;

    int* visited = (int*) calloc(sizeof(int), (size_t)count);
    if (!visited) return ERROR_ALLOC_FAIL;

    for (int i = 0; i < count; i++) {
        weight[i] = SCREEN_BYTES_COUNT;
        visited[i] = 0;
    }

    weight[0] = 0;
    mst[0] = -1;

    int u;
    for (int i = 0; i < count; i++) {
        
        int min_weight = SCREEN_BYTES_COUNT;
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

    return ERROR_NO;
}

static void make_graph(MapRoom_t* map_room, int* graph, Tunnel_t* tunnels)
{
    for (int i = 0; i < map_room->count_rooms; i++) {
        for (int j = i + 1; j < map_room->count_rooms; j++) {
            int min_dist = SCREEN_BYTES_COUNT;
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

static ErrorCode paint_graph(MapRoom_t* map_room, char* map)
{
    ErrorCode error = ERROR_NO;
    int* colors = (int*) calloc(sizeof(int), (size_t)(SCREEN_BYTES_COUNT));
    if (!colors) return ERROR_ALLOC_FAIL;

    int* childs = (int*) calloc(sizeof(int), (size_t)(SCREEN_BYTES_COUNT));
    if (!childs) return ERROR_ALLOC_FAIL;
    
    int color = 1;
    for (int i = 0; i < SCREEN_BYTES_COUNT; i++) {
        if (colors[i] == 0 && obj_can_set(map, i)) {
            childs[color - 1] += paint_subgraph(i, color, colors, map);
            color++;
        }
    }
    int count_rooms = color - 1;

    error = map_room_init(map_room, count_rooms, childs, colors);
    if (error) return error;
    
    free(childs);
    free(colors);

    return ERROR_NO;
}

static int paint_subgraph(int v, int color, int* colors, char* map)
{
    int count_childs = 0;
    colors[v] = color;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (abs(dx) + abs(dy) != 1)
                continue;

            int dpos = v + dy * BYTE_WIDTH + dx;
            if (!is_pos_in_byte_window(dpos))
                    continue;

            if (colors[dpos] == 0 && obj_can_set(map, dpos))
                count_childs += paint_subgraph(dpos, color, colors, map);
        }
    }
    return count_childs + 1;
}

static ErrorCode map_room_init(MapRoom_t* map_room, int count_rooms, int* childs, int* colors)
{
    map_room->count_rooms = count_rooms;
    map_room->obj_count = (int*)  calloc(sizeof(int),  (size_t)(count_rooms));
    if (!map_room->obj_count) return ERROR_ALLOC_FAIL;

    map_room->obj_ind = (Point_t**) calloc(sizeof(Point_t*), (size_t)(count_rooms));
    if (!map_room->obj_ind) return ERROR_ALLOC_FAIL;

    for (int i = 0; i < count_rooms; i++) {
        map_room->obj_ind[i] = (Point_t*) calloc(sizeof(Point_t), (size_t)(childs[i]));
        if (!map_room->obj_ind[i]) return ERROR_ALLOC_FAIL;
    }

    for (int i = 0; i < SCREEN_BYTES_COUNT; i++) {
        if (colors[i] > 0) {
            int ind = map_room->obj_count[colors[i] - 1];

            map_room->obj_ind[colors[i] - 1][ind].x = i % BYTE_WIDTH;
            map_room->obj_ind[colors[i] - 1][ind].y = i / BYTE_WIDTH;
            map_room->obj_ind[colors[i] - 1][ind].ind = i;

            map_room->obj_count[colors[i] - 1]++;
        }
    }
    return ERROR_NO;
}

static void map_room_delete(MapRoom_t* map_room)
{
    for (int i = 0; i < map_room->count_rooms; i++)
        free(map_room->obj_ind[i]);
    free(map_room->obj_ind);
    free(map_room->obj_count);
}

static ErrorCode make_hills(Map_t* map)
{
    ErrorCode error = ERROR_NO;
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++) {
        if (map->map[pos] == SYM_OBJ_WALL) {
            int height = 0;
            error = get_height(map->map, pos, &height);
            if (error) return error;

            map->shadow[pos] = (MAX_HILL_HEIGHT - MIN(MAX_HILL_HEIGHT, height)) * HILL_SLOPE;
        }
    }
    return ERROR_NO;
}

static ErrorCode step_get_height(int* distance, char* map, int x, int y, Queue_t** queue, int* res_step)
{
    ErrorCode error = ERROR_NO;

    int len = distance[BYTE_WIDTH * y + x];
    int* new_queue;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (abs(dx) + abs(dy) == 0)
                continue;

            int dpos = BYTE_WIDTH * (y + dy) + x + dx;

            if (map[dpos] == SYM_OBJ_WALL) {
                new_queue = &distance[dpos];

                if (*new_queue == 0 || *new_queue > len + 1) {
                    *new_queue = len + 1;
                    error = enqueue(queue, x + dx, y + dy);
                    if (error) return error;
                }
            } else {
                *res_step = dpos;
                return ERROR_NO;
            }
        }
    }
    *res_step = -1;
    return ERROR_NO;
}

static ErrorCode get_height(char* map, int pos, int* height)
{
    ErrorCode error = ERROR_NO;
    Queue_t* queue = NULL;

    int distance[SCREEN_BYTES_COUNT] = {};

    distance[pos] = 1;
    error = enqueue(&queue, pos % BYTE_WIDTH, pos / BYTE_WIDTH);
    if (error) return error;

    while (queue) {
        int x, y;
        if (!dequeue(&queue, &x, &y)) {
            
            int res_step = 0;
            error = step_get_height(distance, map, x, y, &queue, &res_step);
            if (error) return error;

            if (res_step != -1) {
                int x1 = pos % BYTE_WIDTH;
                int y1 = pos / BYTE_WIDTH;
                int x2 = res_step % BYTE_WIDTH;
                int y2 = res_step / BYTE_WIDTH;
                *height = MAX(abs(x1 - x2), abs(y1 - y2));
                dtor_queue(queue, queue);
                return ERROR_NO;
            }
        }
    }
    dtor_queue(queue, queue);
    *height = MAX_HILL_HEIGHT;
    return ERROR_NO;
}

static void set_lighting(Map_t* map)
{
    memset(map->light, 25, SCREEN_BYTES_COUNT);
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++)
        if (map->map[pos] == SYM_OBJ_LAMP)
            set_light_lamp(map, pos);

    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++)
        map->light[pos]= (unsigned char)MAX(0, (int)map->light[pos] - (int)map->shadow[pos]);
}

static void set_light_lamp(Map_t* map, int pos)
{
    for (int dy = -light_dist; dy <= light_dist; dy++) {
        for (int dx = -light_dist; dx <= light_dist; dx++) {
            int dpos = pos + dy * BYTE_WIDTH + dx;

            if (!is_pos_in_byte_window(dpos))
                    continue;

            int x = pos % BYTE_WIDTH;
            if (dx + x >= BYTE_WIDTH || dx + x < 0)
                continue;

            float dist = sqrtf((float)(dx * dx + dy * dy));
            unsigned char ratio = 255 * (float)(1.f - MIN(1.f, dist / (float)light_dist / light_force));
            unsigned char ratio_norm = MAX(0, MIN(255, ratio));

            unsigned char was_light = map->light[dpos];
            map->light[dpos] = (unsigned char) MIN(255, was_light + ratio_norm);

            unsigned char was_red   = map->col[dpos * 3 + 0];
            unsigned char was_green = map->col[dpos * 3 + 1];
            map->col[dpos * 3 + 0] = MIN(255, was_red   + ratio_norm / 7);
            map->col[dpos * 3 + 1] = MIN(255, was_green + ratio_norm / 7);
        }
    }
}

static void map_gen(char* map)
{
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++) {
        if (is_obj_on_border(pos))
            map[pos] = SYM_OBJ_BORDER;

        int chance = (rand() % 100 + 1);
        if (chance >= CHANCE_LIFE)
            map[pos] = SYM_OBJ_WALL;
        else
            map[pos] = SYM_OBJ_ROAD;
    }
}

static void map_step(char* map)
{
    char new_m[SCREEN_BYTES_COUNT];
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++) {
        if (is_obj_on_border(pos)) {
            new_m[pos] = SYM_OBJ_BORDER;
            continue;
        }
        int byte_val_neighbours = 0;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (!((dx != 0) || (dy != 0)))
                    continue;
                if (map[pos + dy * BYTE_WIDTH + dx] == SYM_OBJ_WALL)
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
    memcpy(map, new_m, SCREEN_BYTES_COUNT * sizeof(char));
}

static ErrorCode map_fill_empty(char* map, PlayerSet_t* PlayerSet)
{
    int count_free = 0;
    int* frees_ind = (int*) calloc(SCREEN_BYTES_COUNT,  sizeof(int));
    if (!frees_ind) return ERROR_ALLOC_FAIL;

    count_free_pos(map, &count_free, frees_ind);
    
    char* free_pos = (char*) calloc((size_t)count_free, sizeof(char));
    if (!free_pos) return ERROR_ALLOC_FAIL;

    select_free_pos(map, free_pos, count_free, frees_ind, PlayerSet);

    free(free_pos);
    free(frees_ind);
    
    return ERROR_NO;
}

static void count_free_pos(char* map, int* count_free, int* frees_ind)
{
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++) {
        for (int k = 0; k < COUNT_OBJECTS; k++) {
            if (map[pos] == OBJECTS[k].symbol && OBJECTS[k].can_go) {
                frees_ind[*count_free] = pos;
                (*count_free)++;
                break;
            }
        }
    }
}

static void select_free_pos(char* map, char* free_pos, int count_free, int* frees_ind, PlayerSet_t* PlayerSet)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (OBJECTS[i].count <= 0) 
            continue;

        for (int j = 0; j < OBJECTS[i].count; j++) {
            while (1) {
                int pos = rand() % count_free;
                if (!free_pos[pos] && check_neighbors(&OBJECTS[i], map, frees_ind[pos])) {
                    map[frees_ind[pos]] = OBJECTS[i].symbol;

                    if (OBJECTS[i].symbol == SYM_OBJ_PLAYER) {
                        PlayerSet->px = frees_ind[pos] % BYTE_WIDTH;
                        PlayerSet->py = frees_ind[pos] / BYTE_WIDTH;
                    }

                    free_pos[pos] = 1;
                    break;
                }
            }
        }
    }
}

static int check_neighbors(Object_t* src_obj, char* map, int pos)
{
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0)
                continue;

            int dpos = pos + dy * BYTE_WIDTH + dx;
            if (!is_pos_in_byte_window(dpos))
                    continue;

            if (src_obj->symbol == SYM_OBJ_LAMP &&
                (map[dpos] == SYM_OBJ_LAMP || map[dpos] == SYM_OBJ_BORDER))
                return 0;

            if (map[dpos] == SYM_OBJ_TUNNEL)
                return 0;
        }
    }

    if (src_obj->count_neighbors == 0) 
        return 1;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (abs(dx) + abs(dy) == 1) {
                for (int i = 0; i < src_obj->count_neighbors; i++) {
                    for (int j = 0; j < COUNT_OBJECTS; j++) {
                        if (OBJECTS[j].symbol == map[pos + dy * BYTE_WIDTH + dx] &&
                            OBJECTS[j].symbol == src_obj->neighbors[i])
                            return 1;
                    }
                }
            }
        }
    }
    return 0;
}

static void map_write2file(char* map, FILE* file) 
{
    for (int y = 0; y < BYTE_HEIGHT; y++) {
        int y_pos = y * BYTE_WIDTH;
        for (int x = 0; x < BYTE_WIDTH; x++)
            fprintf(file, "%c", map[y_pos + x]);
        fprintf(file, "\n");
    }
}

static int is_obj_on_border(int pos)
{
    int x = pos % BYTE_WIDTH;
    int y = pos / BYTE_WIDTH;
    return (x == 0 || y == 0 || x == BYTE_WIDTH - 1 || y == BYTE_HEIGHT - 1);
}

static int obj_can_set(char* map, int map_ind)
{
    for (int k = 0; k < COUNT_OBJECTS; k++)
        if (map[map_ind] == OBJECTS[k].symbol && !OBJECTS[k].can_go)
            return 0;
    return 1;
}

static int is_pos_in_byte_window(int pos)
{
    return (pos >= 0 && pos < SCREEN_BYTES_COUNT);
}