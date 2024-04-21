#include "Control.h"

void control(SDL_Event* event, PlayerControl_t* PlayerControl)
{
    switch (event->type) {
        case SDL_QUIT:
            PlayerControl->is_exit = 1;
            return;
        case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    PlayerControl->mouse_button_left = 1;
                    break;
                case SDL_BUTTON_RIGHT:
                    break;
                default:
                    break;
            }
            return;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_SPACE:
                    PlayerControl->space = 1;
                    return;
                case SDLK_z:
                    PlayerControl->scale = 1;
                    return;
                case SDLK_x:
                    PlayerControl->scale = -1;
                    return;
                case SDLK_F1:
                    PlayerControl->info = 1;
                    return;
                case SDLK_LEFT : case SDLK_a:
                    PlayerControl->dx = -1;
                    break;
                case SDLK_RIGHT: case SDLK_d:
                    PlayerControl->dx = 1;
                    break;
                case SDLK_UP: case SDLK_w:
                    PlayerControl->dy = -1;
                    break;
                case SDLK_DOWN: case SDLK_s:
                    PlayerControl->dy = 1;
                    break;
                case SDLK_ESCAPE:
                    PlayerControl->is_exit = 1;
                    return;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    PlayerControl->is_exit = 0;
}
