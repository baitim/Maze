#include "Control.h"

static void move_on_valid(char* lab, PlayerSet_t* PlayerSet, float dx, float dy);

int control_event(sf::RenderWindow* window, sf::Event event, char* lab, PlayerSet_t* PlayerSet)
{
    lab[(int)PlayerSet->py * M + (int)PlayerSet->px] = SYM_OBJ_ROAD;
    float dx = 0, dy = 0;
    switch (event.type) {
        case sf::Event::Closed:
            return 1;
        case sf::Event::MouseButtonReleased:
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    PlayerSet->scale *= PlayerSet->Kscale;
                    break;
                case sf::Mouse::Right:
                    PlayerSet->scale /= PlayerSet->Kscale;
                    break;
                default:
                    break;
            }
            break;
        case sf::Event::EventType::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::Space:
                    PlayerSet->is_active_mouse = !PlayerSet->is_active_mouse;
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
                default:
                    break;
            }
            break;
        default:
            break;
    }
    move_on_valid(lab, PlayerSet, dx, dy);
    lab[(int)PlayerSet->py * M + (int)PlayerSet->px] = SYM_OBJ_PLAYER;
    return 0;
}

int control_noevent(sf::RenderWindow* window, char* lab, PlayerSet_t* PlayerSet)
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
    lab[(int)PlayerSet->py * M + (int)PlayerSet->px] = SYM_OBJ_ROAD;
    float dx = 0, dy = 0;
    if (PlayerSet->is_active_mouse) {
        PlayerSet->delay_dx = (PlayerSet->delay_dx + 1) % delay_dx_max;
        PlayerSet->delay_dy = (PlayerSet->delay_dy + 1) % delay_dy_max;
        int XY_dx = PlayerSet->dx * ((PlayerSet->delay_dx == 0) ? 1 : 0);
        int XY_dy = PlayerSet->dy * ((PlayerSet->delay_dy == 0) ? 1 : 0);
        dx = ((int)mouse_pos.x > WIDTH / 2) ? XY_dx : -XY_dx;
        dy = ((int)mouse_pos.y > HEIGHT/ 2) ? XY_dy : -XY_dy;
    }
    move_on_valid(lab, PlayerSet, dx, dy);
    lab[(int)PlayerSet->py * M + (int)PlayerSet->px] = SYM_OBJ_PLAYER;
    return 0;
}

static void move_on_valid(char* lab, PlayerSet_t* PlayerSet, float dx, float dy)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (lab[(int)PlayerSet->py * M + (int)PlayerSet->px + (int)dx] == OBJECTS[i].symbol && OBJECTS[i].can_go) {
            PlayerSet->px += dx;
            break;
        }
    }
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (lab[((int)PlayerSet->py + (int)dy) * M + (int)PlayerSet->px] == OBJECTS[i].symbol && OBJECTS[i].can_go) {
            PlayerSet->py += dy;
            break;
        }
    }
}