#include <string.h>

#include "Output.h"
#include "Math.h"

static void paint_object(bool outside, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                         int iN, int iM, int step_x, int step_y);

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
    int dy0 = HEIGHT / 2 - (HEIGHT / 2 / step_y) * step_y;
    int dx0 = WIDTH  / 2 - (WIDTH  / 2 / step_x) * step_x;

    for (int iy = 0; iy < HEIGHT; iy += step_y) {
        bool outside_y = false;
        int dy = (dy0 + iy - HEIGHT / 2) / step_y;
        int iN = cy + dy;
        if (iN != MIN(N - 1, MAX(0, iN)))
            outside_y = true;
        for (int ix = 0; ix < WIDTH; ix += step_x) {
            bool outside_x = false;
            int dx = (dx0 + ix - WIDTH / 2) / step_x;
            int iM = cx + dx;
            if (iM != MIN(M - 1, MAX(0, iM)))
                outside_x = true;

            bool outside = outside_x || outside_y;
            paint_object(outside, pixels, map, ix, iy, iN, iM, step_x, step_y);
        }
    }
}

static void paint_object(bool outside, sf::Uint8* pixels, Map_t* map, int ix, int iy,
                         int iN, int iM, int step_x, int step_y)
{
    bool is_obj = false;
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (map->lab[iN * M + iM] == OBJECTS[i].symbol) {
            for (int y = 0; y < step_y; y++) {
                for (int x = 0; x < step_x; x++) {
                    int y_col = y * hbyte2pix / step_y;
                    int x_col = x * wbyte2pix / step_x;
                    sf::Uint8* pixel = &pixels[POS(ix + x, iy + y)];
                    if (outside) {
                        memset(pixel, 0, 4 * sizeof(unsigned char));
                    } else {
                        unsigned char* color = (unsigned char*)&OBJECTS[i].bytes_color[(y_col * wbyte2pix + x_col) * 4];
                        memcpy(pixel, color, 3 * sizeof(unsigned char));
                        pixel[3] = (unsigned char)map->light[iN * M + iM];
                    }
                }
            }
            is_obj = true;
            return;
        }
    }
    if (!is_obj) {
        for (int y = 0; y < step_y; y++) {
            sf::Uint8* pixel = &pixels[POS(ix, iy + y)];
            memset(pixel, 0, 4 * sizeof(unsigned char) * step_x);
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