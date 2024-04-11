#ifndef SRC_WINDOW_H
#define SRC_WINDOW_H

#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Control.h"
#include "Errors.h"

ErrorCode window_prepare(sf::RenderWindow* window, sf::Texture* texture, sf::Sprite* sprite,
                         sf::Uint8** pixels, sf::Font* font, char* font_file,
                         sf::Text* POS_Text, sf::Text* FPS_Text);

ErrorCode window_default_loop(sf::RenderWindow* window, sf::Texture* texture, sf::Sprite* sprite,
                              sf::Uint8* pixels, sf::Text* POS_Text, sf::Text* FPS_Text,
                              Map_t* map, PlayerSet_t* PlayerSet, char* screenshot_file);

#endif // SRC_WINDOW_H