#include "Control.h"
#include "Window.h"
#include "FindPath.h"

static void control_mouse_move  ();
static void control_follow_path ();
static void callback_mouse_click();

void control()
{
    switch (event.type) {
        case SDL_QUIT:
            player_set.is_exit = 1;
            return;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    player_set.is_active_mouse_click  = 1;
                    player_set.is_active_mouse_move  = 0;
                    callback_mouse_click();
                    break;
                case SDL_BUTTON_RIGHT:
                    break;
                default:
                    break;
            }
            return;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    player_set.scale *= KScale;
                    if (player_set.is_active_mouse_move) {
                        clean_path(&player_set.path);
                        player_set.is_active_mouse_click = 0;
                    }
                    return;
                case SDLK_z:
                    player_set.scale = 1;
                    return;
                case SDLK_x:
                    player_set.scale = -1;
                    return;
                case SDLK_F1:
                    player_set.is_info = 1;
                    return;
                case SDLK_LEFT : case SDLK_a:
                    player_set.dx = -1;
                    break;
                case SDLK_RIGHT: case SDLK_d:
                    player_set.dx = 1;
                    break;
                case SDLK_UP: case SDLK_w:
                    player_set.dy = -1;
                    break;
                case SDLK_DOWN: case SDLK_s:
                    player_set.dy = 1;
                    break;
                case SDLK_ESCAPE:
                    player_set.is_exit = 1;
                    return;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    if (player_set.dx != 0 || player_set.dy != 0) {
        clean_path(&player_set.path);
        player_set.is_active_mouse_click = 0;
        player_set.is_active_mouse_move  = 0;
    }

    player_set.is_exit = 0;
}

void control_noevent()
{
    if (player_set.is_active_mouse_move)
        control_mouse_move();

    if (player_set.is_active_mouse_click)
        control_follow_path();
}

static void control_mouse_move()
{
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    int dx = 0, dy = 0;
    player_set.delay_dx = (player_set.delay_dx + 1) % delay_dx_max;
    player_set.delay_dy = (player_set.delay_dy + 1) % delay_dy_max;
    int XY_dx = player_set.dx * ((player_set.delay_dx == 0) ? 1 : 0);
    int XY_dy = player_set.dy * ((player_set.delay_dy == 0) ? 1 : 0);
    dx = (mouse_x > PIX_WIDTH  / 2) ? XY_dx : -XY_dx;
    dy = (mouse_y > PIX_HEIGHT / 2) ? XY_dy : -XY_dy;
    
    player_set.dx = dx;
    player_set.dy = dy;
}

static void control_follow_path()
{
    if (!player_set.path.path_exist && player_set.path.passed == player_set.path.count)
        return;
    
    player_set.delay_path = (player_set.delay_path + 1) % delay_path_count;

    if (player_set.path.passed == player_set.path.count) {
        player_set.delay_path = 0;
        clean_path(&player_set.path);
    }

    if (player_set.delay_path == 1) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (abs(dx) + abs(dy) != 1)
                    continue;
                
                int x = player_set.px + dx;
                int y = player_set.py + dy;
                if (y * BYTE_WIDTH + x < 0)
                    continue;

                if (player_set.path.path[y * BYTE_WIDTH + x] == player_set.path.passed + 1) {
                    player_set.dx = dx;
                    player_set.dy = dy;
                }
            }
        }
        player_set.path.passed++;
    }
}

static void callback_mouse_click()
{

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    double dx = (mouse_x < PIX_WIDTH  / 2) ? 1 - wbyte2pix : wbyte2pix - 1;
    double dy = (mouse_y < PIX_HEIGHT / 2) ? 1 - hbyte2pix : hbyte2pix - 1;

    mouse_x = (int)((double)((double)((double)mouse_x - (double)PIX_WIDTH  / 2 + dx) / (double)wbyte2pix / (double)wscale_render) / (double)player_set.scale);
    mouse_y = (int)((double)((double)((double)mouse_y - (double)PIX_HEIGHT / 2 + dy) / (double)hbyte2pix / (double)hscale_render) / (double)player_set.scale);
    mouse_x = player_set.px + mouse_x;
    mouse_y = player_set.py + mouse_y;

    find_shortest_path(mouse_x, mouse_y);
}

int passable_object(char* map, int x, int y)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (map[y * BYTE_WIDTH + x] == (char)OBJECTS[i].symbol && OBJECTS[i].can_go)
            return 1;
    }
    return 0;
}