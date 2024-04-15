#include <SFML/Graphics.hpp>
#include <string.h>

#include "Input.h"
#include "Config.h"

ErrorCode objects_get()
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        sf::Image color_texture;
        int is_load = color_texture.loadFromFile(OBJECTS[i].name_src_file);
        if (!is_load) return ERROR_CANT_LOAD_OBJECT;

        const sf::Uint8* ByteBufferColor = color_texture.getPixelsPtr();
        memcpy(&OBJECTS[i].bytes_color, ByteBufferColor, hbyte2pix * wbyte2pix * 4);
    }
    return ERROR_NO;
}