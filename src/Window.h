#ifndef SRC_WINDOW_H
#define SRC_WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Config.h"
#include "Control.h"
#include "Errors.h"

ErrorCode window_prepare(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                         Uint8** pixels, TTF_Font** font, char* font_file);

ErrorCode window_default_loop(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                              Uint8* pixels, TTF_Font** font, Map_t* map, PlayerSet_t* PlayerSet,
                              char* screenshot_file);

#endif // SRC_WINDOW_H