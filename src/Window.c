#include <string.h>
#include <time.h>

#include "Output.h"
#include "Window.h"

static void lock_texture(SDL_Texture** texture, Uint8** pixels);

static void free_window_stuff(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture,
                              TTF_Font** font, Uint8* pixels, char* pos_string, char* fps_string);

ErrorCode window_prepare(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                         Uint8** pixels, TTF_Font** font, char* font_file)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    *window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               PIX_WIDTH, PIX_HEIGHT, SDL_WINDOW_SHOWN);

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor (*renderer, 0, 0, 0, 0);
    SDL_RenderClear(*renderer);
    SDL_RenderPresent(*renderer);

    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                 PIX_WIDTH, PIX_HEIGHT);

    *pixels = (Uint8*) calloc(sizeof(Uint8), PIX_WIDTH * PIX_HEIGHT * 4);
    if (!(*pixels)) return ERROR_ALLOC_FAIL;

    SDL_RenderSetScale(*renderer, 1, 1);

    *font = TTF_OpenFont(font_file, 25);

    return ERROR_NO;
}

ErrorCode window_default_loop(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                              Uint8* pixels, TTF_Font** font, Map_t* map, PlayerSet_t* PlayerSet,
                              char* screenshot_file)
{
    ErrorCode error = ERROR_NO;
    char* pos_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!pos_string) return ERROR_ALLOC_FAIL;

    char* fps_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!fps_string) return ERROR_ALLOC_FAIL;
    
    SDL_Event event = {};
    while (1) {
        clock_t clock_begin = clock();

        while (SDL_PollEvent(&event)) {
            int is_exit = 0;
            error = control_event(map, PlayerSet, &event, &is_exit);
            if (error) return error;

            if (is_exit) {
                make_screenshot(*renderer, screenshot_file);
                free_window_stuff(window, renderer, texture, font, pixels, pos_string, fps_string);
                return ERROR_NO;
            }
        }
        control_noevent(map, PlayerSet);

        lock_texture(texture, &pixels);
        render_map(pixels, map, PlayerSet);
        SDL_UnlockTexture(*texture);
        SDL_RenderCopy(*renderer, *texture, NULL, NULL);

        clock_t clock_end = clock();
        if (PlayerSet->is_info) 
            print_state_info(renderer, *font, pos_string, fps_string, clock_begin, clock_end, PlayerSet);

        SDL_RenderPresent(*renderer);
    }

    free_window_stuff(window, renderer, texture, font, pixels, pos_string, fps_string);
    return ERROR_NO;
}

static void lock_texture(SDL_Texture** texture, Uint8** pixels)
{
    int pitch = PIX_HEIGHT * PIX_WIDTH * 4;

    void** void_pixels = (void**) pixels;
    SDL_LockTexture(*texture, NULL, void_pixels, &pitch);

    return;
}

static void free_window_stuff(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture,
                              TTF_Font** font, Uint8* pixels, char* pos_string, char* fps_string)
{
    SDL_DestroyWindow(*window);
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyTexture(*texture);
    TTF_CloseFont(*font);
    
    free(pixels);
    free(pos_string);
    free(fps_string);
}