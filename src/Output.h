#ifndef SRC_OUTPUT_H
#define SRC_OUTPUT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Config.h"
#include "Control.h"

void print_help     ();
void sdl_print_text(char* text, SDL_Renderer** renderer, TTF_Font* font, int x, int y, int w, int h);
void render_map     (Uint8* pixels, Map_t* map, PlayerSet_t* PlayerSet);
void make_screenshot(SDL_Renderer* renderer, const char* output_file);

void print_state_info(SDL_Renderer** renderer, TTF_Font* font, char* pos_string, char* fps_string,
                      clock_t clock_begin, clock_t clock_end, PlayerSet_t* PlayerSet);

#endif // SRC_OUTPUT_H