#include <string.h>
#include <time.h>

#include "Output.h"
#include "Window.h"

static void lock_texture(SDL_Texture** texture, Uint8** pixels);

ErrorCode window_prepare(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                         Mix_Music** music, Uint8** pixels, TTF_Font** font, CmdInputData_t* cmd_data)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    *window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               PIX_WIDTH, PIX_HEIGHT, SDL_WINDOW_SHOWN);

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_ADD);
    SDL_RenderClear(*renderer);
    SDL_RenderPresent(*renderer);

    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                                 PIX_WIDTH, PIX_HEIGHT);


    Mix_OpenAudio(AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048);
    *music = Mix_LoadMUS("music/beat.wav");

    *pixels = (Uint8*) calloc(sizeof(Uint8), PIX_WIDTH * PIX_HEIGHT * 4);
    if (!(*pixels)) return ERROR_ALLOC_FAIL;

    SDL_RenderSetScale(*renderer, 1, 1);

    *font = TTF_OpenFont(cmd_data->font_file, 40);

    return ERROR_NO;
}

ErrorCode window_default_loop(SDL_Window** window, SDL_Texture** texture, SDL_Renderer** renderer,
                              Mix_Music** music, Uint8** pixels, TTF_Font** font, Map_t* map,
                              PlayerSet_t* PlayerSet, char* screenshot_file)
{
    Mix_PlayMusic(*music, -1);

    Uint8* texture_pixels = (Uint8*) calloc(sizeof(Uint8), PIX_WIDTH * PIX_HEIGHT * 4);
    if (!texture_pixels) return ERROR_ALLOC_FAIL;

    ErrorCode error = ERROR_NO;
    char* pos_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!pos_string) return ERROR_ALLOC_FAIL;

    char* fps_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!fps_string) return ERROR_ALLOC_FAIL;

    char* count_coins_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!fps_string) return ERROR_ALLOC_FAIL;
    
    double old_fps = 0.f;
    clock_t clock_begin = 0, clock_end = 0;
    SDL_Event event = {};
    while (1) {
        while (SDL_PollEvent(&event)) {
            int is_exit = 0;
            error = control_event(map, PlayerSet, &event, &is_exit, renderer);
            if (error) goto finally;

            if (is_exit) {
                make_screenshot(*renderer, screenshot_file);
                goto finally;
            }
        }
        control_noevent(map, PlayerSet, renderer);
        
        lock_texture(texture, &texture_pixels);
        render_map(*pixels, map, PlayerSet);
        memcpy(texture_pixels, *pixels, PIX_WIDTH * PIX_HEIGHT * 4 * sizeof(Uint8));
        SDL_UnlockTexture(*texture);
        SDL_RenderCopy(*renderer, *texture, NULL, NULL);

        clock_end = clock();
        if (PlayerSet->is_info) 
            print_state_info(renderer, *font, pos_string, fps_string, count_coins_string,
                             clock_begin, clock_end, PlayerSet, &old_fps);
        clock_begin = clock();

        SDL_RenderPresent(*renderer);
    }

finally:
    free(texture_pixels);
    free(pos_string);
    free(fps_string);
    free(count_coins_string);
    return error;
}

static void lock_texture(SDL_Texture** texture, Uint8** pixels)
{
    int pitch = PIX_HEIGHT * PIX_WIDTH * 4;

    void** void_pixels = (void**) pixels;
    SDL_LockTexture(*texture, NULL, void_pixels, &pitch);

    return;
}

void free_window_stuff(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture,
                       TTF_Font* font, Mix_Music* music, Uint8* pixels)
{
    free(pixels);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    Mix_FreeMusic(music);

    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}