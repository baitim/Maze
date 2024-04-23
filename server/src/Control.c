#include <stdlib.h>

#include "Control.h"

void set_player_start_pos(PlayerSet_t* player, char* map)
{
    int count_free = 0;

    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++)
        if (map[pos] == SYM_OBJ_ROAD)
            count_free++;
    
    int ind_player = rand() % count_free;
    count_free = 0;
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++) {
        if (map[pos] == SYM_OBJ_ROAD) {
            if (count_free == ind_player) {
                player->px = pos % BYTE_WIDTH;
                player->py = pos / BYTE_WIDTH;
                break;
            }
            count_free++;
        }
    }
}