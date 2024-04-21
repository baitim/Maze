#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#include "ProcessObject.h"

static double wav_duration(const char *filename);

void process_object(SDL_Renderer** renderer)
{
    if (map_info.map[player_set.py * BYTE_WIDTH + player_set.px] == SYM_OBJ_COIN) {
        Mix_Chunk* sound_coin = NULL;
        sound_coin = Mix_LoadWAV("music/sound_coin.wav");
        Mix_PlayChannel(-1, sound_coin, 0);

        player_set.count_coins++;
    }

    if (map_info.map[player_set.py * BYTE_WIDTH + player_set.px] == SYM_OBJ_SCREAM) {
        SDL_Surface* screamer = IMG_Load("images/Screamer.png");
        SDL_Texture* texture = SDL_CreateTextureFromSurface(*renderer, screamer);

        SDL_RenderCopy(*renderer, texture, NULL, NULL);
        SDL_RenderPresent(*renderer);

        Mix_Chunk* sound_screamer = NULL;
        sound_screamer = Mix_LoadWAV("music/sound_screamer.wav");
        Mix_PlayChannel(-1, sound_screamer, 0);

        double duration_screamer = wav_duration("music/sound_screamer.wav");
        clock_t clock_begin = clock();
        clock_t clock_end   = clock();
        double elapsed_s = 0;
        while (elapsed_s < duration_screamer) {
            clock_end = clock();
            elapsed_s = (double)(clock_end - clock_begin) / (double)(CLOCKS_PER_SEC);
        }
    }
}

static double wav_duration(const char *filename)
{
	SDL_AudioSpec spec;
	uint32_t audioLen;
	uint8_t *audioBuf;
	double seconds = 0.0;

	SDL_LoadWAV(filename, &spec, &audioBuf, &audioLen);

	SDL_FreeWAV(audioBuf);
	uint32_t sampleSize = SDL_AUDIO_BITSIZE(spec.format) / 8;
	uint32_t sampleCount = audioLen / sampleSize;

	uint32_t sampleLen = 0;
	if(spec.channels)
		sampleLen = sampleCount / spec.channels;
	else
		sampleLen = sampleCount;
	
    seconds = (double)sampleLen / AUDIO_FREQUENCY;
	return seconds;
}