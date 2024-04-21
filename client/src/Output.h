#ifndef CLIENT_SRC_OUTPUT_H
#define CLIENT_SRC_OUTPUT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Errors.h"

typedef struct InfoStrings_t_ {
    char* pos_string;
    char* fps_string;
    char* count_coins_string;
} InfoStrings_t;

void print_help     ();
void sdl_print_text(char* text, SDL_Renderer** renderer, TTF_Font* font, int x, int y, int w, int h);
void render_map     (Uint8* pixels);
void make_screenshot(SDL_Renderer* renderer, const char* output_file);

void print_state_info(SDL_Renderer** renderer, TTF_Font* font, char* pos_string, char* fps_string,
                      char* count_coins_string, clock_t clock_begin, clock_t clock_end, double* old_fps);

ErrorCode info_strings_init  (InfoStrings_t* info_strings);
void      info_strings_delete(InfoStrings_t* info_strings);

int pos_in_pix_window(int x, int y);

#endif // CLIENT_SRC_OUTPUT_H