#include "Control.h"
#include "FindPath.h"

static void control_mouse_move  (sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet);
static void control_follow_path (Map_t* map, PlayerSet_t* PlayerSet);
static ErrorCode callback_mouse_click(sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet);
static void move_on_valid       (char* map, PlayerSet_t* PlayerSet, int dx, int dy);

ErrorCode control_event(sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet,
                        sf::Event* event, int* is_exit)
{
    ErrorCode error = ERROR_NO;

    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_ROAD;
    int dx = 0, dy = 0;
    switch (event->type) {
        case sf::Event::Closed:
            *is_exit = 1;
            return ERROR_NO;
        case sf::Event::MouseButtonReleased:
            switch (event->mouseButton.button) {
                case sf::Mouse::Left:
                    PlayerSet->is_active_mouse_click  = true;
                    PlayerSet->is_active_mouse_move = false;
                    error = callback_mouse_click(window, map, PlayerSet);
                    if (error) return error;
                    break;
                case sf::Mouse::Right:
                    break;
                default:
                    break;
            }
            break;
        case sf::Event::EventType::KeyPressed:
            switch (event->key.code) {
                case sf::Keyboard::Space:
                    PlayerSet->is_active_mouse_move  = !PlayerSet->is_active_mouse_move;
                    if (PlayerSet->is_active_mouse_move) {
                        clean_path(&map->path);
                        PlayerSet->is_active_mouse_click = false;
                    }
                    break;
                case sf::Keyboard::Z:
                    PlayerSet->scale *= PlayerSet->Kscale;
                    break;
                case sf::Keyboard::X:
                    PlayerSet->scale /= PlayerSet->Kscale;
                    break;
                case sf::Keyboard::F1:
                    PlayerSet->is_info = !PlayerSet->is_info;
                    break;
                case sf::Keyboard::Left : case sf::Keyboard::A:
                    dx -= PlayerSet->dx;
                    break;
                case sf::Keyboard::Right: case sf::Keyboard::D:
                    dx += PlayerSet->dx;
                    break;
                case sf::Keyboard::Up: case sf::Keyboard::W:
                    dy -= PlayerSet->dy;
                    break;
                case sf::Keyboard::Down: case sf::Keyboard::S:
                    dy += PlayerSet->dy;
                    break;
                case sf::Keyboard::Escape:
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
        PlayerSet->is_active_mouse_click = false;
        PlayerSet->is_active_mouse_move = false;
    }
    move_on_valid(map->map, PlayerSet, dx, dy);
    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_PLAYER;

    *is_exit = 0;
    return ERROR_NO;
}

void control_noevent(sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet)
{
    if (PlayerSet->is_active_mouse_move)
        control_mouse_move(window, map, PlayerSet);

    if (PlayerSet->is_active_mouse_click)
        control_follow_path(map, PlayerSet);
}

static void control_mouse_move(sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet)
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_ROAD;
    int dx = 0, dy = 0;
    
    PlayerSet->delay_dx = (PlayerSet->delay_dx + 1) % delay_dx_max;
    PlayerSet->delay_dy = (PlayerSet->delay_dy + 1) % delay_dy_max;
    int XY_dx = PlayerSet->dx * ((PlayerSet->delay_dx == 0) ? 1 : 0);
    int XY_dy = PlayerSet->dy * ((PlayerSet->delay_dy == 0) ? 1 : 0);
    dx = (mouse_pos.x > PIX_WIDTH  / 2) ? XY_dx : -XY_dx;
    dy = (mouse_pos.y > PIX_HEIGHT / 2) ? XY_dy : -XY_dy;
    
    move_on_valid(map->map, PlayerSet, dx, dy);
    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_PLAYER;
}

static void control_follow_path(Map_t* map, PlayerSet_t* PlayerSet)
{
    if (!map->path.path_exist && map->path.passed == map->path.count)
        return;

    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_ROAD;
    
    PlayerSet->delay_path = (PlayerSet->delay_path + 1) % delay_path;

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

    map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] = SYM_OBJ_PLAYER;
}

static ErrorCode callback_mouse_click(sf::RenderWindow* window, Map_t* map, PlayerSet_t* PlayerSet)
{
    ErrorCode error = ERROR_NO;
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);

    int dx = (mouse_pos.x < PIX_WIDTH  / 2) ? 1 - wbyte2pix : wbyte2pix - 1;
    int dy = (mouse_pos.y < PIX_HEIGHT / 2) ? 1 - hbyte2pix : hbyte2pix - 1;

    mouse_pos.x = (int)((float)((mouse_pos.x - PIX_WIDTH  / 2 + dx) / wbyte2pix / wscale_render) / PlayerSet->scale);
    mouse_pos.y = (int)((float)((mouse_pos.y - PIX_HEIGHT / 2 + dy) / hbyte2pix / hscale_render) / PlayerSet->scale);
    mouse_pos.x = PlayerSet->px + mouse_pos.x;
    mouse_pos.y = PlayerSet->py + mouse_pos.y;

    error = find_shortest_path(map, PlayerSet, &mouse_pos);
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
        if (map[y * BYTE_WIDTH + x] == OBJECTS[i].symbol && OBJECTS[i].can_go)
            return 1;
    }
    return 0;
}