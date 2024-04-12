#include <string.h>

#include "ANSI_colors.h"
#include "Output.h"
#include "Math.h"
#include "Life.h"
#include "ProcessCmd.h"

static void paint_object(bool outside, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                         int pos, int step_x, int step_y, int chunk_x, int chunk_y);

static void paint_path(sf::Uint8* pixels, Map_t* map, int ix, int iy,
                       int step_x, int step_y, int chunk_x, int chunk_y);

static void paint_path_target(int is_exist, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                              int step_x, int step_y, int chunk_x, int chunk_y);

static void paint_obj_chunk(sf::Uint8* pixels, int ix, int iy, int step_x, int step_y,
                            int chunk_x, int chunk_y, int obj_ind, int (*skip)(unsigned char*));

static int  obj_skip_white(unsigned char* color);

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

void set_text(sf::Font* font, sf::Text* text, float x, float y)
{
    text->setFont(*font);
    text->setPosition(x, y);
    text->setCharacterSize(28);
    text->setOutlineColor(sf::Color(252, 0, 17));
}

void render_lab(sf::Uint8* pixels, Map_t* map, PlayerSet_t* PlayerSet)
{
    int cx = PlayerSet->px;
    int cy = PlayerSet->py;
    int step_x = (int) (PlayerSet->scale * wscale_render * wbyte2pix);
    int step_y = (int) (PlayerSet->scale * hscale_render * hbyte2pix);
    int dy0 = PIX_HEIGHT / 2 - (PIX_HEIGHT / 2 / step_y) * step_y;
    int dx0 = PIX_WIDTH  / 2 - (PIX_WIDTH  / 2 / step_x) * step_x;

    int chunk_y = step_y;
    for (int iy = 0; iy < PIX_HEIGHT; iy += step_y) {
        if (iy + step_y >= PIX_HEIGHT) 
            chunk_y = PIX_HEIGHT - iy;
        
        int dy = (dy0 + iy - PIX_HEIGHT / 2) / step_y;
        int iN = cy + dy;
        bool outside_y = false;
        if (iN != MIN(BYTE_HEIGHT - 1, MAX(0, iN)))
            outside_y = true;

        int chunk_x = step_x;
        for (int ix = 0; ix < PIX_WIDTH; ix += step_x) {
            if (ix + step_x >= PIX_WIDTH) 
                chunk_x = PIX_WIDTH - ix;

            int dx = (dx0 + ix - PIX_WIDTH / 2) / step_x;
            int iM = cx + dx;
            bool outside_x = false;
            if (iM != MIN(BYTE_WIDTH - 1, MAX(0, iM)))
                outside_x = true;

            int pos = iN * BYTE_WIDTH + iM;

            bool outside = outside_x || outside_y;
            paint_object(outside, pixels, map, ix, iy, pos, step_x, step_y, chunk_x, chunk_y);

            if (!outside && map->path.path[pos] > 0 && map->path.path_target != pos &&
                map->path.passed < map->path.path[pos])
                paint_path(pixels, map, ix, iy, step_x, step_y, chunk_x, chunk_y);

            if (!outside && map->path.path_target == pos &&
                ((map->path.passed < map->path.count && map->path.path_exist) || !map->path.path_exist))
                paint_path_target(map->path.path_exist, pixels, map, ix, iy, step_x, step_y, chunk_x, chunk_y);
        }
    }
}

static void paint_object(bool outside, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                         int pos, int step_x, int step_y, int chunk_x, int chunk_y)
{
    int obj_ind = SYM_OBJ_ERR;
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (map->map[pos] == OBJECTS[i].symbol) {
            obj_ind = i;
            break;
        }
    }

    if (outside || obj_ind == SYM_OBJ_ERR) {
        for (int y = 0; y < step_y; y++) {
            sf::Uint8* pixel = &pixels[pos_in_pix_window(ix, iy + y)];
            memset(pixel, 0, 4 * sizeof(sf::Uint8) * step_x);
        }
        return;
    }
            
    for (int y = 0; y < chunk_y; y++) {
        int y_col = y * hbyte2pix / step_y;
        for (int x = 0; x < chunk_x; x++) {
            int x_col = x * wbyte2pix / step_x;

            sf::Uint8* pixel = &pixels[pos_in_pix_window(ix + x, iy + y)];
            unsigned char* color = &OBJECTS[obj_ind].bytes_color[(y_col * wbyte2pix + x_col) * 4];

            pixel[0] = MIN(255, color[0] + map->col[pos * 3 + 0]);
            pixel[1] = MIN(255, color[1] + map->col[pos * 3 + 1]);
            pixel[2] = MIN(255, color[2] + map->col[pos * 3 + 2]);

            pixel[3] = map->light[pos];
        }
    }
}

static void paint_path(sf::Uint8* pixels, Map_t* map, int ix, int iy,
                       int step_x, int step_y, int chunk_x, int chunk_y)
{
    int ind_obj_path = -1;
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (OBJECTS[i].symbol == SYM_OBJ_PATH) {
            ind_obj_path = i;
            break;
        }
    }

    paint_obj_chunk(pixels, ix, iy, step_x, step_y, chunk_x, chunk_y, ind_obj_path, obj_skip_white);
}

static void paint_path_target(int is_exist, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                              int step_x, int step_y, int chunk_x, int chunk_y)
{
    int ind_obj_path_target = -1;
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if ((is_exist && OBJECTS[i].symbol  == SYM_OBJ_DEST) ||
            (!is_exist && OBJECTS[i].symbol == SYM_OBJ_IMDEST)) {
            ind_obj_path_target = i;
            break;
        }
    }

    paint_obj_chunk(pixels, ix, iy, step_x, step_y, chunk_x, chunk_y, ind_obj_path_target, obj_skip_white);
}

static void paint_obj_chunk(sf::Uint8* pixels, int ix, int iy, int step_x, int step_y,
                            int chunk_x, int chunk_y, int obj_ind, int (*skip)(unsigned char*))
{
    for (int y = 0; y < chunk_y; y++) {
        int y_col = y * hbyte2pix / step_y;
        for (int x = 0; x < chunk_x; x++) {
            int x_col = x * wbyte2pix / step_x;
            sf::Uint8* pixel = &pixels[pos_in_pix_window(ix + x, iy + y)];

            unsigned char* color = &OBJECTS[obj_ind].bytes_color[(y_col * wbyte2pix + x_col) * 4];

            if (skip(color))
                continue;
    
            pixel[0] = MIN(255, color[0] + pixel[0]);
            pixel[1] = MIN(255, color[1] + pixel[1]);
            pixel[2] = MIN(255, color[2] + pixel[2]);
        }
    }
}

static int obj_skip_white(unsigned char* color)
{
    return (color[0] == 255 && color[1] == 255 && color[2] == 255);
}

void make_screenshot(sf::RenderWindow* window, const char* output_file)
{
    sf::Vector2u windowSize = window->getSize();
    sf::Texture texture2;
    texture2.create(windowSize.x, windowSize.y);
    texture2.update(*window);
    sf::Image image = texture2.copyToImage();
    image.saveToFile(output_file);
}

void print_state_info(sf::RenderWindow* window, sf::Text* POS_Text, sf::Text* FPS_Text,
                      char* pos_string, int len_pos_string, char* fps_string, int len_fps_string,
                      std::chrono::_V2::steady_clock::time_point clock_begin,
                      std::chrono::_V2::steady_clock::time_point clock_end,
                      PlayerSet_t* PlayerSet)
{
    snprintf(pos_string, len_pos_string, "pos: %d  %d", PlayerSet->px, PlayerSet->py);
    POS_Text->setString(pos_string);
    window->draw(*POS_Text);

    auto elapsed_ms = std::chrono::duration<double,std::milli>(clock_end - clock_begin).count();
    double fps = 100.f / elapsed_ms;
    snprintf(fps_string, len_fps_string, "fps: %.f", fps);
    FPS_Text->setString(fps_string);
    window->draw(*FPS_Text);
}