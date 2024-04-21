#include <string.h>
#include <pthread.h>
#include <SDL2/SDL_image.h>

#include "ANSI_colors.h"
#include "Output.h"
#include "ProcessCmd.h"
#include "Config.h"
#include "Control.h"

Object_t OBJECTS[COUNT_OBJECTS] = {
    {SYM_OBJ_WALL,    0,  "images/Texture/TextureWall.png",     },
    {SYM_OBJ_ROAD,    1,  "images/Texture/TextureRoad.png",     },
    {SYM_OBJ_BORDER,  0,  "images/Texture/TextureBorder.png",   },
    {SYM_OBJ_PLAYER,  1,  "images/Texture/TexturePlayer.png",   },
    {SYM_OBJ_COIN,    1,  "images/Texture/TextureCoin.png",     },
    {SYM_OBJ_LAMP,    0,  "images/Texture/TextureLamp.png",     },
    {SYM_OBJ_PATH,    1,  "images/Texture/TexturePath.png",     },
    {SYM_OBJ_DEST,    1,  "images/Texture/TextureDest.png",     },
    {SYM_OBJ_IMDEST,  1,  "images/Texture/TextureImDest.png",   },
    {SYM_OBJ_TUNNEL,  1,  "images/Texture/TextureRoad.png",     },
    {SYM_OBJ_ERR,     0,  "images/Texture/TextureError.png",    },
    {SYM_OBJ_SCREAM,  1,  "images/Texture/TextureScreamer.png", }
};

typedef struct BlockRenderInfo_t_ {
    Uint8* pixels;
    int obj_size_x;
    int obj_size_y;
    int dx0;
    int dy0;
    int cx;
    int cy;
    int num_thread;
    int thread_step;
} BlockRenderInfo_t;

void* render_block(void* line_render_info);

static void paint_object(int outside, Uint8* pixels, int ix, int iy,
                         int pos, int obj_size_x, int obj_size_y, int chunk_x, int chunk_y);

static void paint_path(Uint8* pixels, int ix, int iy, int obj_size_x, int obj_size_y,
                       int chunk_x, int chunk_y, Uint8* light, int pos);

static void paint_path_target(int is_exist, Uint8* pixels, int ix, int iy, Uint8* light, int pos,
                              int obj_size_x, int obj_size_y, int chunk_x, int chunk_y);

static void paint_chunk(Uint8* pixels, int ix, int iy, int obj_size_x, int obj_size_y,
                        int chunk_x, int chunk_y, int obj_ind, Uint8* light, int pos);

static int get_obj_index (int obj_sym);

static void win_print_text(char* text, SDL_Renderer** renderer, TTF_Font* font,
                           int x, int y, int w, int h);

void print_help()
{
    printf(print_lgreen("Help:\n"));
    printf(print_lgreen("flag:                     info:\n"));
    for (int i = 0; i < COUNT_OPTIONS; i++) {
        int count_print = 0;
        printf(print_lgreen("%s%n"), OPTIONS[i].name, &count_print);
        for (int j = 0; j <= MAX_CMD_COMMAND_SIZE - count_print; j++)
            printf(" ");
        printf(print_lgreen("%s\n"), OPTIONS[i].description);
    }
}

void render_map(Uint8* pixels)
{
    pthread_t pool[RENDER_THREADS];

    int cx = map_info.px;
    int cy = map_info.py;
    int obj_size_x = (int) (player_set.scale * wscale_render * wbyte2pix);
    int obj_size_y = (int) (player_set.scale * hscale_render * hbyte2pix);
    int dx0 = PIX_WIDTH  / 2 - (PIX_WIDTH  / 2 / obj_size_x) * obj_size_x;
    int dy0 = PIX_HEIGHT / 2 - (PIX_HEIGHT / 2 / obj_size_y) * obj_size_y;

    int thread_step = PIX_HEIGHT / RENDER_THREADS / obj_size_y;

    BlockRenderInfo_t block_render_info[RENDER_THREADS];

    for (int num_thread = 0; num_thread < RENDER_THREADS; num_thread++) {
        block_render_info[num_thread] = 
            (BlockRenderInfo_t) {.pixels = pixels,
             .obj_size_x = obj_size_x, .obj_size_y = obj_size_y,
             .dx0 = dx0, .dy0 = dy0, .cx = cx, .cy = cy,
             .num_thread = num_thread, .thread_step = thread_step};

        pthread_create(&pool[num_thread], NULL, render_block, (void*)&block_render_info[num_thread]);
    }

    for (int num_thread = 0; num_thread < RENDER_THREADS; num_thread++)
        pthread_join(pool[num_thread], NULL);
}

void* render_block(void* block_render_info)
{
    BlockRenderInfo_t* render_data = (BlockRenderInfo_t*) block_render_info;

    Uint8* pixels = render_data->pixels;
    int obj_size_x = render_data->obj_size_x;
    int obj_size_y = render_data->obj_size_y;
    int dx0 = render_data->dx0;
    int dy0 = render_data->dy0;
    int cx  = render_data->cx;
    int cy  = render_data->cy;
    int num_thread  = render_data->num_thread;
    int thread_step = render_data->thread_step;

    int start_iy = thread_step * num_thread * obj_size_y;
    int end_iy   = start_iy + thread_step * obj_size_y;

    if (num_thread == RENDER_THREADS - 1)
        end_iy = PIX_HEIGHT;

    int chunk_y  = obj_size_y;
    for (int iy = start_iy; iy < end_iy; iy += obj_size_y) {
        if (iy + obj_size_y >= PIX_HEIGHT)
            chunk_y = PIX_HEIGHT - iy - 1;

        int dy = (dy0 + iy - PIX_HEIGHT / 2) / obj_size_y;
        int iN = cy + dy;
        int outside_y = 0;
        if (iN != MIN(BYTE_HEIGHT - 1, MAX(0, iN)))
            outside_y = 1;

        int chunk_x = obj_size_x;
        for (int ix = 0; ix < PIX_WIDTH; ix += obj_size_x) {
            if (ix + obj_size_x >= PIX_WIDTH) 
                chunk_x = PIX_WIDTH - ix;

            int dx = (dx0 + ix - PIX_WIDTH / 2) / obj_size_x;
            int iM = cx + dx;
            int outside_x = 0;
            if (iM != MIN(BYTE_WIDTH - 1, MAX(0, iM)))
                    outside_x = 1;

            int pos = iN * BYTE_WIDTH + iM;
            int outside = (outside_x || outside_y) ? 1 : 0;

            paint_object(outside, pixels, ix, iy, pos, obj_size_x, obj_size_y, chunk_x, chunk_y);

            if (!outside && player_set.path.path[pos] > 0 && player_set.path.path_target != pos &&
                player_set.path.passed < player_set.path.path[pos])
                paint_path(pixels, ix, iy, obj_size_x, obj_size_y, chunk_x, chunk_y, map_info.light, pos);

            if (!outside && player_set.path.path_target == pos &&
                ((player_set.path.passed < player_set.path.count && player_set.path.path_exist) || !player_set.path.path_exist))
                paint_path_target(player_set.path.path_exist, pixels, ix, iy, map_info.light, pos,
                                  obj_size_x, obj_size_y, chunk_x, chunk_y);
        }
    }

    return NULL;
}

static void paint_object(int outside, Uint8* pixels, int ix, int iy,
                         int pos, int obj_size_x, int obj_size_y, int chunk_x, int chunk_y)
{
    int obj_ind = get_obj_index(map_info.map[pos]);

    if (outside || obj_ind == SYM_OBJ_ERR) {
        for (int y = 0; y < chunk_y; y++) {
            Uint8* pixel = &pixels[pos_in_pix_window(ix, iy + y)];
            memset(pixel, 0, 4 * sizeof(Uint8) * (size_t)chunk_x);
        }
        return;
    }

    int pos_col = pos * 3;
    float x_coef = (float)wbyte2pix / (float)obj_size_x;
    float y_coef = (float)hbyte2pix / (float)obj_size_y;
    for (int y = 0; y < chunk_y; y++) {

        int y_col = (int)((float)y * y_coef) * wbyte2pix * 4;
        int y_pix_window = (iy + y) * PIX_WIDTH * 4;

        for (int x = 0; x < chunk_x; x++) {

            int x_col = (int)((float)x * x_coef) * 4;
            int x_pix_window = (ix + x) * 4;

            Uint8* pixel = &pixels[y_pix_window + x_pix_window];
            unsigned char* color = &OBJECTS[obj_ind].bytes_color[y_col + x_col];

            float light_norm = (float)map_info.light[pos] / 255.f;

            pixel[0] = (Uint8)((float)MIN(255, color[0] + map_info.col[pos_col + 0]) * light_norm);
            pixel[1] = (Uint8)((float)MIN(255, color[1] + map_info.col[pos_col + 1]) * light_norm);
            pixel[2] = (Uint8)((float)MIN(255, color[2] + map_info.col[pos_col + 2]) * light_norm);
        }
    }
}

static void paint_path(Uint8* pixels, int ix, int iy, int obj_size_x, int obj_size_y,
                       int chunk_x, int chunk_y, Uint8* light, int pos)
{
    int ind_obj_path = get_obj_index(SYM_OBJ_PATH);

    paint_chunk(pixels, ix, iy, obj_size_x, obj_size_y, chunk_x, chunk_y, ind_obj_path, light, pos);
}

static void paint_path_target(int is_exist, Uint8* pixels, int ix, int iy, Uint8* light, int pos,
                              int obj_size_x, int obj_size_y, int chunk_x, int chunk_y)
{
    int ind_obj_path_target = -1;
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if ((is_exist && OBJECTS[i].symbol  == SYM_OBJ_DEST) ||
            (!is_exist && OBJECTS[i].symbol == SYM_OBJ_IMDEST)) {
            ind_obj_path_target = i;
            break;
        }
    }

    paint_chunk(pixels, ix, iy, obj_size_x, obj_size_y, chunk_x, chunk_y,
                ind_obj_path_target, light, pos);
}

static void paint_chunk(Uint8* pixels, int ix, int iy, int obj_size_x, int obj_size_y,
                            int chunk_x, int chunk_y, int obj_ind, Uint8* light, int pos)
{
    float x_coef = (float)wbyte2pix / (float)obj_size_x;
    float y_coef = (float)hbyte2pix / (float)obj_size_y;
    for (int y = 0; y < chunk_y; y++) {

        int y_col = (int)((float)y * y_coef) * wbyte2pix * 4;
        int y_pix_window = (iy + y) * PIX_WIDTH * 4;

        for (int x = 0; x < chunk_x; x++) {

            int x_col = (int)((float)x * x_coef) * 4;
            int x_pix_window = (ix + x) * 4;

            Uint8* pixel = &pixels[y_pix_window + x_pix_window];
            unsigned char* color = &OBJECTS[obj_ind].bytes_color[y_col + x_col];

            if (color[0] == 255 && color[1] == 255 && color[2] == 255)
                continue;

            float light_norm = (float)light[pos] / 255.f;
                   
            pixel[0] = (Uint8)((float)MIN(255, color[0] + pixel[0]) * light_norm);
            pixel[1] = (Uint8)((float)MIN(255, color[1] + pixel[1]) * light_norm);
            pixel[2] = (Uint8)((float)MIN(255, color[2] + pixel[2]) * light_norm);
        }
    }
}

static int get_obj_index(int obj_sym)
{
    for (int i = 0; i < COUNT_OBJECTS; i++)
        if ((int)OBJECTS[i].symbol == obj_sym)
            return i;
    return -1;
}

void make_screenshot(SDL_Renderer* renderer, const char* output_file)
{
    SDL_Rect viewport;
    SDL_Surface* surface = NULL;
    SDL_RenderGetViewport(renderer, &viewport);
    surface = SDL_CreateRGBSurface(0, viewport.w, viewport.h, 32, 0, 0, 0, 0);
  
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

    IMG_SavePNG(surface, output_file);
    SDL_FreeSurface(surface);
}

static void win_print_text(char* text, SDL_Renderer** renderer, TTF_Font* font,
                           int x_, int y_, int w_, int h_)
{
    SDL_Color color = {255, 0, 17, 255};

    SDL_Surface* info = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(*renderer, info);

    SDL_Rect rect = {.x = x_, .y = y_, .w = w_, .h = h_};
    
    SDL_RenderCopy(*renderer, texture, NULL, &rect);

    SDL_FreeSurface(info);
    SDL_DestroyTexture(texture);
}

void print_state_info(SDL_Renderer** renderer, TTF_Font* font, char* pos_string, char* fps_string,
                      char* count_coins_string, clock_t clock_begin, clock_t clock_end, double* old_fps)
{
    player_set.delay_info = (player_set.delay_info + 1) % delay_info_count;

    snprintf(pos_string, MAX_SIZE_INFO_STR, "pos: %d  %d", map_info.px, map_info.py);

    if (player_set.delay_info == 1) {
        double elapsed_ms = (double)(clock_end - clock_begin) / (double)(CLOCKS_PER_SEC / 1000);
        double fps = 1000.f / elapsed_ms;
        *old_fps = fps;
    }
    snprintf(fps_string, MAX_SIZE_INFO_STR, "fps: %.f", *old_fps);

    snprintf(count_coins_string, MAX_SIZE_INFO_STR, "coins: %d", map_info.count_coins);

    win_print_text(pos_string, renderer, font, 10, 10, 180, 50);
    win_print_text(fps_string, renderer, font, 10, 65, 125, 45);
    win_print_text(count_coins_string, renderer, font, 10, 110, 150, 40);
}

ErrorCode info_strings_init(InfoStrings_t* info_strings)
{
    info_strings->pos_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!info_strings->pos_string) return ERROR_ALLOC_FAIL;

    info_strings->fps_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!info_strings->fps_string) return ERROR_ALLOC_FAIL;

    info_strings->count_coins_string = (char*) calloc(MAX_SIZE_INFO_STR, sizeof(char));
    if (!info_strings->fps_string) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

void info_strings_delete(InfoStrings_t* info_strings)
{
    free(info_strings->pos_string);
    free(info_strings->fps_string);
    free(info_strings->count_coins_string);
}

int pos_in_pix_window(int x, int y)
{
    return (y * PIX_WIDTH + x) * 4;
}