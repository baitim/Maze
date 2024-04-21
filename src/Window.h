#ifndef SRC_WINDOW_H
#define SRC_WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "Config.h"
#include "Control.h"
#include "Errors.h"
#include "ProcessCmd.h"

ErrorCode window_prepare(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                         Mix_Music** music, Uint8** pixels, TTF_Font** font, CmdInputData_t* cmd_data);

ErrorCode window_default_loop(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                              Mix_Music** music, Uint8** pixels, TTF_Font** font, Map_t* map,
                              PlayerSet_t* PlayerSet, char* screenshot_file);

void free_window_stuff(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture,
                       TTF_Font* font, Mix_Music* music, Uint8* pixels);

#endif // SRC_WINDOW_H