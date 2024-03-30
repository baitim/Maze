#ifndef OUTPUT_H
#define OUTPUT_H

#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Control.h"

void paint_pixel(sf::Uint8* pixel, int r, int g, int b);
void render_full_lab(sf::Uint8* pixels, char* lab);
void render_lab(sf::Uint8* pixels, char* lab, XYset_t* XYset);

#endif // OUTPUT_H