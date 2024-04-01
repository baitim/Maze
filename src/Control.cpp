#include "Control.h"

static void move_on_valid(char* lab, XYset_t* XYset, float dx, float dy);

int control_event(sf::RenderWindow* window, sf::Event event, char* lab, XYset_t* XYset)
{
    lab[(int)XYset->py * M + (int)XYset->px] = SYM_OBJ_ROAD;
    float dx = 0, dy = 0;
    switch (event.type) {
        case sf::Event::Closed:
            return 1;
        case sf::Event::MouseButtonReleased:
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    XYset->scale *= XYset->Kscale;
                    break;
                case sf::Mouse::Right:
                    XYset->scale /= XYset->Kscale;
                    break;
                default:
                    break;
            }
            break;
        case sf::Event::EventType::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::Space:
                    XYset->is_active_mouse = !XYset->is_active_mouse;
                    break;
                case sf::Keyboard::F1:
                    XYset->is_info = !XYset->is_info;
                    break;
                case sf::Keyboard::Left : case sf::Keyboard::A:
                    dx -= XYset->dx;
                    break;
                case sf::Keyboard::Right: case sf::Keyboard::D:
                    dx += XYset->dx;
                    break;
                case sf::Keyboard::Up: case sf::Keyboard::W:
                    dy -= XYset->dy;
                    break;
                case sf::Keyboard::Down: case sf::Keyboard::S:
                    dy += XYset->dy;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    move_on_valid(lab, XYset, dx, dy);
    lab[(int)XYset->py * M + (int)XYset->px] = SYM_OBJ_PLAYER;
    return 0;
}

int control_noevent(sf::RenderWindow* window, char* lab, XYset_t* XYset)
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
    lab[(int)XYset->py * M + (int)XYset->px] = SYM_OBJ_ROAD;
    float dx = 0, dy = 0;
    if (XYset->is_active_mouse) {
        XYset->delay_dx = (XYset->delay_dx + 1) % delay_dx_max;
        XYset->delay_dy = (XYset->delay_dy + 1) % delay_dy_max;
        int XY_dx = XYset->dx * ((XYset->delay_dx == 0) ? 1 : 0);
        int XY_dy = XYset->dy * ((XYset->delay_dy == 0) ? 1 : 0);
        dx = ((int)mouse_pos.x > WIDTH / 2) ? XY_dx : -XY_dx;
        dy = ((int)mouse_pos.y > HEIGHT/ 2) ? XY_dy : -XY_dy;
    }
    move_on_valid(lab, XYset, dx, dy);
    lab[(int)XYset->py * M + (int)XYset->px] = SYM_OBJ_PLAYER;
    return 0;
}

static void move_on_valid(char* lab, XYset_t* XYset, float dx, float dy)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (lab[(int)XYset->py * M + (int)XYset->px + (int)dx] == OBJECTS[i].symbol && OBJECTS[i].can_go) {
            XYset->px += dx;
            break;
        }
    }
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (lab[((int)XYset->py + (int)dy) * M + (int)XYset->px] == OBJECTS[i].symbol && OBJECTS[i].can_go) {
            XYset->py += dy;
            break;
        }
    }
}