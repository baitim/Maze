#ifndef SRC_OUTPUT_H
#define SRC_OUTPUT_H

#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Control.h"

void print_help     ();
void set_text       (sf::Font* font, sf::Text* text, float x, float y);
void render_lab     (sf::Uint8* pixels, Map_t* map, PlayerSet_t* PlayerSet);
void make_screenshot(sf::RenderWindow* window, const char* output_file);

#endif // SRC_OUTPUT_H