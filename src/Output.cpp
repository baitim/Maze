#include <string.h>

#include "Output.h"

static void paint_object(sf::Uint8* pixels, char* lab, int ix, int iy,
                         int iN, int iM, int step_x, int step_y);

void render_lab(sf::Uint8* pixels, char* lab, PlayerSet_t* PlayerSet)
{
    int cx = PlayerSet->px;
    int cy = PlayerSet->py;
    int step_x = PlayerSet->scale * wscale_render * wbyte2pix;
    int step_y = PlayerSet->scale * hscale_render * hbyte2pix;
    int dy0 = HEIGHT / 2 - (int)(HEIGHT / 2 / step_y) * step_y;
    int dx0 = WIDTH  / 2 - (int)(WIDTH  / 2 / step_x) * step_x;
    for (int iy = 0; iy < HEIGHT; iy += step_y) {
        int dy = (dy0 + iy - HEIGHT / 2) / step_y;
        int iN = MIN(N - 1, MAX(0, cy + dy));
        for (int ix = 0; ix < WIDTH; ix += step_x) {
            int dx = (dx0 + ix - WIDTH / 2) / step_x;
            int iM = MIN(M - 1, MAX(0, cx + dx));

            paint_object(pixels, lab, ix, iy, iN, iM, step_x, step_y);
        }
    }
}

static void paint_object(sf::Uint8* pixels, char* lab, int ix, int iy,
                         int iN, int iM, int step_x, int step_y)
{
    for (int i = 0; i < COUNT_OBJECTS; i++) {
        if (lab[iN * M + iM] == OBJECTS[i].symbol) {
            for (int y = 0; y < step_y; y++) {
                for (int x = 0; x < step_x; x++) {
                    int y_col = y * hbyte2pix / step_y;
                    int x_col = x * wbyte2pix / step_x;
                    sf::Uint8* pixel = &pixels[POS(ix + x, iy + y)];
                    char* color = &OBJECTS[i].bytes_color[(y_col * wbyte2pix + x_col) * 4];
                    memcpy(pixel, color, 3 * sizeof(char));
                }
            }
            break;
        }
    }
}