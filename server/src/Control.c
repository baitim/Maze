#include "time.h"

#include "Control.h"

void set_player_start_pos(PlayerSet_t* player, char* map)
{
    int count_free = 0;

    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++)
        if (map[pos] == SYM_OBJ_ROAD)
            count_free++;
    
    for (int pos = 0; pos < SCREEN_BYTES_COUNT; pos++) {
        if (map[pos] == SYM_OBJ_ROAD && pos == count_free) {
            player->px = pos % BYTE_WIDTH;
            player->py = pos / BYTE_WIDTH;

            count_free++;
            break;
        }
    }
}