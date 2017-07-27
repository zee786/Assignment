#pragma once

#include <stdint.h>
#include "KimbleBase.h"

class KimbleBase;
class Test
{
public:
	int8_t kill_opponent_peg(KimbleBase *game);
    int8_t occupy_block_on_board(KimbleBase *game);
    int8_t free_block_on_board(KimbleBase *game);
    int8_t enter_finish_lane(KimbleBase *game);
    int8_t pop_a_peg(KimbleBase *game);
    int8_t board_single_move_test(KimbleBase *game);

};

