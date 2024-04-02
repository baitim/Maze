#ifndef OUTPUT_H
#define OUTPUT_H

#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Control.h"

void render_lab(sf::Uint8* pixels, Map_t* map, PlayerSet_t* PlayerSet);
void make_screenshot(sf::RenderWindow* window, const char* output_file);

#endif // OUTPUT_H