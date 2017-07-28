#include "StdAfx.h"

#include <stdio.h>
#include "Test.h"
#include "KimbleBase.h"

int8_t Test::kill_opponent_peg(KimbleBase *game)
{
    game->setup_board(game->board);
    Player_t *player_1, *player_2;
    game->players.access_player_data(0, player_1);
    game->players.access_player_data(1, player_2);

    player_1->meta_data.pegs[0].peg_position = 10;
    player_1->meta_data.pegs_at_home-=1;
    player_1->meta_data.pegs_in_normal_lane+=1;
    player_1->meta_data.pegs[0].peg_state = IN_CIRCULATION;

    game->board->pos_normal_lane[10].occupied = true;
    game->board->pos_normal_lane[10].occupant[0].peg_id = 0;
    game->board->pos_normal_lane[10].occupant[0].player_id = 0;
    game->board->pos_normal_lane[10].pos = 10;

    player_2->meta_data.pegs[1].peg_position = 8;
    player_2->meta_data.pegs_at_home-=1;
    player_2->meta_data.pegs_in_normal_lane+=1;
    player_2->meta_data.pegs[1].peg_state = IN_CIRCULATION;

    game->board->pos_normal_lane[8].occupied = true;
    game->board->pos_normal_lane[8].occupant[0].peg_id  = 1;
    game->board->pos_normal_lane[8].occupant[0].player_id = 1;
    game->board->pos_normal_lane[8].pos = 8;


    game->move_peg_to_position(player_2, 1, 2, IN_CIRCULATION);

    //player_1 peg should die
    if (player_1->meta_data.pegs[0].peg_state == IN_HOME) {
        return SUCCESS;
    }

    return -1;
}

int8_t Test::occupy_block_on_board(KimbleBase *game)
{
    game->setup_board(game->board);
    Player_t *player;
    game->players.access_player_data(0, player);

    player->meta_data.pegs[0].peg_position = 10;
    player->meta_data.pegs_at_home -= 1;
    player->meta_data.pegs_in_normal_lane += 1;
    player->meta_data.pegs[0].peg_state = IN_CIRCULATION;

    game->occupy_block_on_board(player, 12, 0, false);

    if (game->board->pos_normal_lane[12].occupied == true
            && game->board->pos_normal_lane[12].occupant[0].player_id == player->player_id) {
        return SUCCESS;
    }

    return -1;
}

int8_t Test::free_block_on_board(KimbleBase *game)
{
    game->setup_board(game->board);
    Player_t *player;
    game->players.access_player_data(0, player);

    player->meta_data.pegs[0].peg_position = 10;
    player->meta_data.pegs_at_home -= 1;
    player->meta_data.pegs_in_normal_lane += 1;
    player->meta_data.pegs[0].peg_state = IN_CIRCULATION;

    game->occupy_block_on_board(player, 12, 0, false);

    game->free_block_on_board(player, 12, 0, false);

    if (game->board->pos_normal_lane[12].occupied == false
            && game->board->pos_normal_lane[12].occupant[0].player_id != player->player_id) {
        return SUCCESS;
    }

    return -1;
}

int8_t Test::enter_finish_lane(KimbleBase *game)
{
    game->setup_board(game->board);
    Player_t *player = NULL;

    game->players.access_player_data(RED, player);

    player->meta_data.pegs[1].peg_position = RED_ZONE_ORIGIN-2;
    player->meta_data.pegs_at_home -= 1;
    player->meta_data.pegs_in_normal_lane += 1;
    player->meta_data.pegs[1].distance_covered = 25;
    player->meta_data.pegs[1].distance_to_origin_marker = 2;
    player->meta_data.pegs_at_home = 3;
    player->meta_data.pegs_in_finish_lane = 0;
    player->meta_data.pegs_in_normal_lane = 1;
    player->meta_data.pegs[1].peg_state = IN_CIRCULATION;

    game->occupy_block_on_board(player, (RED_ZONE_ORIGIN-2), 1, false);

    game->move_peg_to_position(player, 1, 3, IN_CIRCULATION);

    if (player->meta_data.pegs_at_home == 3
            &&  player->meta_data.pegs_in_finish_lane == 1
            &&  player->meta_data.pegs_in_normal_lane == 0) {
        return SUCCESS;
    }

    return -1;

}

int8_t Test::board_single_move_test(KimbleBase *game)
{
    game->setup_board(game->board);
    Player_t *player = NULL;

    game->players.access_player_data(RED, player);

    while (player->meta_data.pegs[0].peg_state != POPPED_OUT) {
        game->move_peg_to_position(player, player->meta_data.pegs[0].peg_number, 1, player->meta_data.pegs[0].peg_state);
    }

    return SUCCESS;
}

int8_t Test::pop_a_peg(KimbleBase *game)
{
    game->setup_board(game->board);
    Player_t *player = NULL;

    game->players.access_player_data(RED, player);

    player->meta_data.pegs[1].peg_position = RED_ZONE_ORIGIN-2;
    player->meta_data.pegs_at_home -= 1;
    player->meta_data.pegs_in_normal_lane += 1;
    player->meta_data.pegs[1].distance_covered = 25;
    player->meta_data.pegs[1].distance_to_origin_marker = 2;
    player->meta_data.pegs_at_home = 3;
    player->meta_data.pegs_in_finish_lane = 0;
    player->meta_data.pegs_in_normal_lane = 1;
    player->meta_data.pegs[1].peg_state = IN_CIRCULATION;

    game->occupy_block_on_board(player, (RED_ZONE_ORIGIN-2), 1, false);

    game->move_peg_to_position(player, 1, 7, IN_CIRCULATION);

    if (player->meta_data.pegs_at_home == 3
            &&  player->meta_data.pegs_in_finish_lane == 0
            &&  player->meta_data.pegs_in_normal_lane == 1) {
        return SUCCESS;
    }

    return -1;
}