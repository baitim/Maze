#include <string.h>

#include "Input.h"
#include "Config.h"

void objects_get()
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        sf::Image color_texture;
        color_texture.loadFromFile(OBJECTS[i].name_src_file);
        const sf::Uint8* ByteBufferColor = color_texture.getPixelsPtr();
        memcpy(&OBJECTS[i].bytes_color, ByteBufferColor, hbyte2pix * wbyte2pix * 4);
    }
}