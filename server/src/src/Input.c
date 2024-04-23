#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>

#include "Input.h"
#include "Config.h"

ErrorCode objects_get()
{
    SDL_Surface* img = NULL;
    SDL_Surface* rgbaImage = NULL;
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        img = IMG_Load(OBJECTS[i].name_src_file);
        rgbaImage = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA32, 0);
        memcpy(&OBJECTS[i].bytes_color, rgbaImage->pixels, hbyte2pix * wbyte2pix * 4);
    }
    SDL_FreeSurface(img);
    SDL_FreeSurface(rgbaImage);
    return ERROR_NO;
}