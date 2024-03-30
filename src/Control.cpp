#include "Control.h"

Object OBJECTS[COUNT_OBJECTS] = {
    {SYM_OBJ_WALL,      false,  "images/Texture/TextureWall.png"},
    {SYM_OBJ_ROAD,      true,   "images/Texture/TextureRoad.png"},
    {SYM_OBJ_BORDER,    false,  "images/Texture/TextureBorder.png"},
    {SYM_OBJ_PLAYER,    false,  "images/Texture/TexturePlayer.png"},
    {SYM_OBJ_COIN,      true,   "images/Texture/TextureCoin.png"}
};

int control(sf::RenderWindow* window, sf::Event event, char* lab, XYset_t* XYset)
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
    lab[XYset->py * M + XYset->px] = SYM_OBJ_ROAD;
    int dx = 0, dy = 0;
    switch (event.type) {
        case sf::Event::Closed:
            return 1;
        case sf::Event::MouseButtonReleased:
            switch (event.mouseButton.button) {
                case sf::Mouse::Left: case sf::Mouse::Right:
                    break;
                default:
                    break;
            }
            break;
        case sf::Event::EventType::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::Space:
                    dx = ((int)mouse_pos.x > WIDTH / 2) ? XYset->dx : -XYset->dx;
                    dy = ((int)mouse_pos.y > HEIGHT/ 2) ? XYset->dy : -XYset->dy;
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
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (lab[XYset->py * M + XYset->px + dx] == OBJECTS[i].symbol && OBJECTS[i].can_go) {
            XYset->px += dx;
            break;
        }
    }
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (lab[(XYset->py + dy) * M + XYset->px] == OBJECTS[i].symbol && OBJECTS[i].can_go) {
            XYset->py += dy;
            break;
        }
    }
    lab[XYset->py * M + XYset->px] = SYM_OBJ_PLAYER;
    return 0;
}