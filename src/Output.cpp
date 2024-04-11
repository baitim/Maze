#include <string.h>

#include "ANSI_colors.h"
#include "Output.h"
#include "Math.h"
#include "Life.h"
#include "ProcessCmd.h"

static void paint_path(sf::Uint8* pixels, Map_t* map, int ix, int iy,
                       int iN, int iM, int step_x, int step_y, unsigned char* color);

static void paint_object(bool outside, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                         int iN, int iM, int step_x, int step_y, unsigned char* color);

static void paint_path_target(int is_exist, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                              int iN, int iM, int step_x, int step_y, unsigned char* color);

void print_help()
{
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
    unsigned char* color = (unsigned char*) calloc(3, sizeof(unsigned char));
    int cx = PlayerSet->px;
    int cy = PlayerSet->py;
    int step_x = (int) (PlayerSet->scale * wscale_render * wbyte2pix);
    int step_y = (int) (PlayerSet->scale * hscale_render * hbyte2pix);
    int dy0 = PIX_HEIGHT / 2 - (PIX_HEIGHT / 2 / step_y) * step_y;
    int dx0 = PIX_WIDTH  / 2 - (PIX_WIDTH  / 2 / step_x) * step_x;

    for (int iy = 0; iy < PIX_HEIGHT; iy += step_y) {
        bool outside_y = false;
        int dy = (dy0 + iy - PIX_HEIGHT / 2) / step_y;
        int iN = cy + dy;
        if (iN != MIN(BYTE_HEIGHT - 1, MAX(0, iN)))
            outside_y = true;
        for (int ix = 0; ix < PIX_WIDTH; ix += step_x) {
            bool outside_x = false;
            int dx = (dx0 + ix - PIX_WIDTH / 2) / step_x;
            int iM = cx + dx;
            if (iM != MIN(BYTE_WIDTH - 1, MAX(0, iM)))
                outside_x = true;

            bool outside = outside_x || outside_y;
            paint_object(outside, pixels, map, ix, iy, iN, iM, step_x, step_y, color);
            if (!outside &&
                map->path.path[iN * BYTE_WIDTH + iM] > 0 &&
                map->path.path_target != iN * BYTE_WIDTH + iM &&
                map->path.passed < map->path.path[iN * BYTE_WIDTH + iM]) {

                paint_path(pixels, map, ix, iy, iN, iM, step_x, step_y, color);
            }
            if (!outside &&
                map->path.path_target == iN * BYTE_WIDTH + iM &&
                ((map->path.passed < map->path.count && map->path.path_exist) || !map->path.path_exist))

                paint_path_target(map->path.path_exist, pixels, map, ix, iy, iN, iM,
                                  step_x, step_y, color);
        }
    }
}

static void paint_object(bool outside, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                         int iN, int iM, int step_x, int step_y, unsigned char* color)
{
    bool is_obj = false;
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (map->map[iN * BYTE_WIDTH + iM] == OBJECTS[i].symbol) {
            for (int y = 0; y < step_y; y++) {
                for (int x = 0; x < step_x; x++) {
                    int y_col = y * hbyte2pix / step_y;
                    int x_col = x * wbyte2pix / step_x;
                    sf::Uint8* pixel = &pixels[pos_in_pix_window(ix + x, iy + y)];
                    if (outside) {
                        memset(pixel, 0, 4 * sizeof(unsigned char));
                    } else {
                        memcpy(color, &OBJECTS[i].bytes_color[(y_col * wbyte2pix + x_col) * 4], 3 * sizeof(unsigned char));

                        color[0] = MAX(0, MIN(255, color[0] + map->col[(iN * BYTE_WIDTH + iM) * 3 + 0]));
                        color[1] = MAX(0, MIN(255, color[1] + map->col[(iN * BYTE_WIDTH + iM) * 3 + 1]));
                        color[2] = MAX(0, MIN(255, color[2] + map->col[(iN * BYTE_WIDTH + iM) * 3 + 2]));

                        memcpy(pixel, color, 3 * sizeof(unsigned char));
                        pixel[3] = map->light[iN * BYTE_WIDTH + iM];
                    }
                }
            }
            is_obj = true;
            return;
        }
    }
    if (!is_obj) {
        for (int y = 0; y < step_y; y++) {
            sf::Uint8* pixel = &pixels[pos_in_pix_window(ix, iy + y)];
            memset(pixel, 0, 4 * sizeof(unsigned char) * step_x);
        }
    }
}

static void paint_path(sf::Uint8* pixels, Map_t* map, int ix, int iy,
                       int iN, int iM, int step_x, int step_y, unsigned char* color)
{
    int ind_obj_path = -1;
    for (int i = 0; i < COUNT_OBJECTS; i++)
        if (OBJECTS[i].symbol == SYM_OBJ_PATH)
            ind_obj_path = i;

    for (int y = 0; y < step_y; y++) {
        for (int x = 0; x < step_x; x++) {
            int y_col = y * hbyte2pix / step_y;
            int x_col = x * wbyte2pix / step_x;
            sf::Uint8* pixel = &pixels[pos_in_pix_window(ix + x, iy + y)];

            memcpy(color, &OBJECTS[ind_obj_path].bytes_color[(y_col * wbyte2pix + x_col) * 4], 3 * sizeof(unsigned char));

            if (color[0] == 255 && color[1] == 255 && color[2] == 255)
                continue;
    
            color[0] = MAX(0, MIN(255, color[0] + pixel[0]));
            color[1] = MAX(0, MIN(255, color[1] + pixel[1]));
            color[2] = MAX(0, MIN(255, color[2] + pixel[2]));

            memcpy(pixel, color, 3 * sizeof(unsigned char));
        }
    }
}

static void paint_path_target(int is_exist, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                              int iN, int iM, int step_x, int step_y, unsigned char* color)
{
    int ind_obj_path_target = -1;
    for (int i = 0; i < COUNT_OBJECTS; i++)
        if ((is_exist && OBJECTS[i].symbol  == SYM_OBJ_DEST) ||
            (!is_exist && OBJECTS[i].symbol == SYM_OBJ_IMDEST))
            ind_obj_path_target = i;

    for (int y = 0; y < step_y; y++) {
        for (int x = 0; x < step_x; x++) {
            int y_col = y * hbyte2pix / step_y;
            int x_col = x * wbyte2pix / step_x;
            sf::Uint8* pixel = &pixels[pos_in_pix_window(ix + x, iy + y)];

            memcpy(color, &OBJECTS[ind_obj_path_target].bytes_color[(y_col * wbyte2pix + x_col) * 4], 3 * sizeof(unsigned char));

            if (color[0] == 255 && color[1] == 255 && color[2] == 255)
                continue;
    
            color[0] = MAX(0, MIN(255, color[0] + pixel[0]));
            color[1] = MAX(0, MIN(255, color[1] + pixel[1]));
            color[2] = MAX(0, MIN(255, color[2] + pixel[2]));

            memcpy(pixel, color, 3 * sizeof(unsigned char));
        }
    }
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