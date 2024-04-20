#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "ProcessObject.h"

void process_object(Map_t* map, PlayerSet_t* PlayerSet)
{
    if (map->map[PlayerSet->py * BYTE_WIDTH + PlayerSet->px] == SYM_OBJ_COIN) {
        Mix_Chunk* sound_coin = NULL;
        sound_coin = Mix_LoadWAV("music/sound_coin.wav");
        Mix_PlayChannel(-1, sound_coin, 0);

        PlayerSet->count_coins++;
    }
}