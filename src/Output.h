#ifndef OUTPUT_H
#define OUTPUT_H

#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Control.h"

void render_lab(sf::Uint8* pixels, char* lab, XYset_t* XYset);

#endif // OUTPUT_H