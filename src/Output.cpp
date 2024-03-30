#include "Output.h"

void paint_pixel(sf::Uint8* pixel, int r, int g, int b) 
{
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
}

void render_full_lab(sf::Uint8* pixels, char* lab)
{
    for (int iy = 0; iy < HEIGHT; iy += hbyte2pix) {
        int iN = iy / hbyte2pix;
        for (int ix = 0; ix < WIDTH; ix += wbyte2pix) {
            int iM = ix / wbyte2pix;
            for (int i = 0; i < COUNT_OBJECTS; i++) {
                if (lab[iN * M + iM] == OBJECTS[i].symbol) {
                    for (int y = 0; y < hbyte2pix; y++) {
                        for (int x = 0; x < wbyte2pix; x++) {
                            int r = OBJECTS[i].bytes_color[(y * wbyte2pix + x) * 4 + 0];
                            int g = OBJECTS[i].bytes_color[(y * wbyte2pix + x) * 4 + 1];
                            int b = OBJECTS[i].bytes_color[(y * wbyte2pix + x) * 4 + 2];
                            paint_pixel(&pixels[POS(ix + x, iy + y)], r, g, b);
                        }
                    }
                }
            }
        }
    }
}

void render_lab(sf::Uint8* pixels, char* lab, XYset_t* XYset)
{
    int cx = XYset->px;
    int cy = XYset->py;
    int step_x = wscale_render * wbyte2pix;
    int step_y = hscale_render * hbyte2pix;
    int dy0 = HEIGHT / 2 - (int)(HEIGHT / 2 / step_y) * step_y;
    int dx0 = WIDTH  / 2 - (int)(WIDTH  / 2 / step_x) * step_x;
    for (int iy = 0; iy < HEIGHT; iy += step_y) {
        int dy = (dy0 + iy - HEIGHT / 2) / step_y;
        int iN = MIN(N - 1, MAX(0, cy + dy));
        for (int ix = 0; ix < WIDTH; ix += step_x) {
            int dx = (dx0 + ix - WIDTH / 2) / step_x;
            int iM = MIN(M - 1, MAX(0, cx + dx));
            for (int i = 0; i < COUNT_OBJECTS; i++) {
                if (lab[iN * M + iM] == OBJECTS[i].symbol) {
                    for (int y = 0; y < step_y; y++) {
                        for (int x = 0; x < step_x; x++) {
                            int y_col = y * hbyte2pix / step_y;
                            int x_col = x * wbyte2pix / step_x;
                            int r = OBJECTS[i].bytes_color[(y_col * wbyte2pix + x_col) * 4 + 0];
                            int g = OBJECTS[i].bytes_color[(y_col * wbyte2pix + x_col) * 4 + 1];
                            int b = OBJECTS[i].bytes_color[(y_col * wbyte2pix + x_col) * 4 + 2];
                            paint_pixel(&pixels[POS(ix + x, iy + y)], r, g, b);
                        }
                    }
                }
            }
        }
    }
}