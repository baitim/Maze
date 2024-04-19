#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>

#include "Input.h"
#include "Config.h"

ErrorCode objects_get()
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        SDL_Surface* img = IMG_Load(OBJECTS[i].name_src_file);
        SDL_Surface* rgbaImage = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA32, 0);
        memcpy(&OBJECTS[i].bytes_color, rgbaImage->pixels, hbyte2pix * wbyte2pix * 4);
    }
    return ERROR_NO;
}