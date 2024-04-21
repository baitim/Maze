#include "Control.h"
#include "FindPath.h"
#include "ProcessObject.h"

static void control_mouse_move  (Map_t* map, PlayerSet_t* PlayerSet, SDL_Renderer** renderer);
static void control_follow_path (Map_t* map, PlayerSet_t* PlayerSet, SDL_Renderer** renderer);
static ErrorCode callback_mouse_click(Map_t* map, PlayerSet_t* PlayerSet);
static void move_on_valid       (char* map, PlayerSet_t* PlayerSet, int dx, int dy);

ErrorCode control_event(Map_t* map, PlayerSet_t* PlayerSet, SDL_Event* event, int* is_exit, SDL_Renderer** renderer)
{
    ErrorCode error = ERROR_NO;

    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_ROAD;
    int dx = 0, dy = 0;
    switch (event->type) {
        case SDL_QUIT:
            *is_exit = 1;
            return ERROR_NO;
        case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    PlayerSet->is_active_mouse_click  = 1;
                    PlayerSet->is_active_mouse_move  = 0;
                    error = callback_mouse_click(map, PlayerSet);
                    if (error) return error;
                    break;
                case SDL_BUTTON_RIGHT:
                    break;
                default:
                    break;
            }
            return ERROR_NO;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_SPACE:
                    PlayerSet->is_active_mouse_move  = !PlayerSet->is_active_mouse_move;
                    if (PlayerSet->is_active_mouse_move) {
                        clean_path(&map->path);
                        PlayerSet->is_active_mouse_click = 0;
                    }
                    return ERROR_NO;
                case SDLK_z:
                    PlayerSet->scale *= PlayerSet->Kscale;
                    return ERROR_NO;
                case SDLK_x:
                    PlayerSet->scale /= PlayerSet->Kscale;
                    return ERROR_NO;
                case SDLK_F1:
                    PlayerSet->is_info = !PlayerSet->is_info;
                    return ERROR_NO;
                case SDLK_LEFT : case SDLK_a:
                    dx -= PlayerSet->dx;
                    break;
                case SDLK_RIGHT: case SDLK_d:
                    dx += PlayerSet->dx;
                    break;
                case SDLK_UP: case SDLK_w:
                    dy -= PlayerSet->dy;
                    break;
                case SDLK_DOWN: case SDLK_s:
                    dy += PlayerSet->dy;
                    break;
                case SDLK_ESCAPE:
                    *is_exit = 1;
                    return ERROR_NO;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    if (dx != 0 || dy != 0) {
        clean_path(&map->path);
        PlayerSet->is_active_mouse_click = 0;
        PlayerSet->is_active_mouse_move = 0;
    }
    move_on_valid(map->map, PlayerSet, dx, dy);

    process_object(renderer, map, PlayerSet);
    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_PLAYER;

    *is_exit = 0;
    return ERROR_NO;
}

void control_noevent(Map_t* map, PlayerSet_t* PlayerSet, SDL_Renderer** renderer)
{
    if (PlayerSet->is_active_mouse_move)
        control_mouse_move(map, PlayerSet, renderer);

    if (PlayerSet->is_active_mouse_click)
        control_follow_path(map, PlayerSet, renderer);
}

static void control_mouse_move(Map_t* map, PlayerSet_t* PlayerSet, SDL_Renderer** renderer)
{
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_ROAD;
    int dx = 0, dy = 0;
    
    PlayerSet->delay_dx = (PlayerSet->delay_dx + 1) % delay_dx_max;
    PlayerSet->delay_dy = (PlayerSet->delay_dy + 1) % delay_dy_max;
    int XY_dx = PlayerSet->dx * ((PlayerSet->delay_dx == 0) ? 1 : 0);
    int XY_dy = PlayerSet->dy * ((PlayerSet->delay_dy == 0) ? 1 : 0);
    dx = (mouse_x > PIX_WIDTH  / 2) ? XY_dx : -XY_dx;
    dy = (mouse_y > PIX_HEIGHT / 2) ? XY_dy : -XY_dy;
    
    move_on_valid(map->map, PlayerSet, dx, dy);

    process_object(renderer, map, PlayerSet);
    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_PLAYER;
}

static void control_follow_path(Map_t* map, PlayerSet_t* PlayerSet, SDL_Renderer** renderer)
{
    if (!map->path.path_exist && map->path.passed == map->path.count)
        return;

    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_ROAD;
    
    PlayerSet->delay_path = (PlayerSet->delay_path + 1) % delay_path_count;

    if (PlayerSet->delay_path == 1) {
        int new_x = 0;
        int new_y = 0;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (abs(dx) + abs(dy) != 1)
                    continue;
                
                int x = PlayerSet->px + dx;
                int y = PlayerSet->py + dy;
                if (y * BYTE_WIDTH + x < 0)
                    continue;

                if (map->path.path[y * BYTE_WIDTH + x] == map->path.passed + 1) {
                    new_x = x;
                    new_y = y;
                }
            }
        }
        map->path.passed++;
        PlayerSet->px = new_x;
        PlayerSet->py = new_y;
    }

    if (map->path.passed == map->path.count) {
        PlayerSet->delay_path = 0;
        clean_path(&map->path);
    }

    process_object(renderer, map, PlayerSet);

    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_PLAYER;
}

static ErrorCode callback_mouse_click(Map_t* map, PlayerSet_t* PlayerSet)
{
    ErrorCode error = ERROR_NO;

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    double dx = (mouse_x < PIX_WIDTH  / 2) ? 1 - wbyte2pix : wbyte2pix - 1;
    double dy = (mouse_y < PIX_HEIGHT / 2) ? 1 - hbyte2pix : hbyte2pix - 1;

    mouse_x = (int)((double)((double)((double)mouse_x - (double)PIX_WIDTH  / 2 + dx) / (double)wbyte2pix / (double)wscale_render) / (double)PlayerSet->scale);
    mouse_y = (int)((double)((double)((double)mouse_y - (double)PIX_HEIGHT / 2 + dy) / (double)hbyte2pix / (double)hscale_render) / (double)PlayerSet->scale);
    mouse_x = PlayerSet->px + mouse_x;
    mouse_y = PlayerSet->py + mouse_y;

    error = find_shortest_path(map, PlayerSet, mouse_x, mouse_y);
    if (error) return error;

    return ERROR_NO;
}

static void move_on_valid(char* map, PlayerSet_t* PlayerSet, int dx, int dy)
{
    if (passable_object(map, PlayerSet->px + dx, PlayerSet->py))
        PlayerSet->px += dx;

    if (passable_object(map, PlayerSet->px, PlayerSet->py + dy))
        PlayerSet->py += dy;
}

int passable_object(char* map, int x, int y)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (map[y * BYTE_WIDTH + x] == (char)OBJECTS[i].symbol && OBJECTS[i].can_go)
            return 1;
    }
    return 0;
}