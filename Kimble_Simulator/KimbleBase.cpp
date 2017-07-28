#include "StdAfx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "KimbleBase.h"
#include "Utilities.h"

static char buf[100];
uint8_t remaining_players_on_board;
// Seed the random generator
void seed_srand()
{
    srand((unsigned)time(NULL));
}

int roll_die()
{
    return (rand() %  DIE_FACES) + 1;
}

KimbleBase::KimbleBase(uint8_t num_players)
{
    
	number_of_players = num_players;
	remaining_players_on_board=num_players;
    turn_seq = new Turn_sequence_t[num_players];
    board = new Ground_t;
    seed_srand();
    setup_board(board);
}

KimbleBase::~KimbleBase()
{
    delete[] turn_seq;
    delete board;
}

void KimbleBase::setup_board(Ground_t *board)
{
    for (uint8_t i=0; i < NORMAL_LANE_BLOCKS; i++) {
        board->pos_normal_lane[i].in_finish_lane = false;
        board->pos_normal_lane[i].occupied = false;
        // At max a block on the board can have four pegs (of same kind at the same time)
        for (uint8_t j=0; j<MAX_NUMBER_OF_PEGS; j++) {
            board->pos_normal_lane[i].occupant[j].peg_id = -1;
            board->pos_normal_lane[i].occupant[j].player_id = -1;
        }
        board->pos_normal_lane[i].pos = i;

    }

    for (uint8_t i=0; i < FINISH_LANE_BLOCKS; i++) {
        board->pos_finish_lane[i].in_finish_lane = true;
        board->pos_finish_lane[i].occupied = false;
        for (uint8_t j=0; j<MAX_NUMBER_OF_PEGS; j++) {
            board->pos_finish_lane[i].occupant[j].peg_id = -1;
            board->pos_finish_lane[i].occupant[j].player_id = -1;
          }
        board->pos_finish_lane[i].pos = i;
    }
}

void KimbleBase::game_engine_log(Player_config_t *config, const char *msg)
{
    // check if its a general engine log
    if (!config) {
        LOG(ANSI_COLOR_GRAY "[Engine]: %s\n" ANSI_COLOR_RESET, msg);
        return;
    }

    // otherwise its Player log
    switch (config->peg_color) {
        case BLUE:
            LOG(ANSI_COLOR_CYAN "[%s]: %s\n" ANSI_COLOR_RESET, config->player_name, msg);
            break;
        case GREEN:
            LOG(ANSI_COLOR_GREEN "[%s]: %s\n" ANSI_COLOR_RESET, config->player_name, msg);
            break;
        case RED:
            LOG(ANSI_COLOR_RED "[%s]: %s\n" ANSI_COLOR_RESET, config->player_name, msg);
            break;
        case YELLOW:
            LOG(ANSI_COLOR_YELLOW "[%s]: %s\n" ANSI_COLOR_RESET, config->player_name, msg);
            break;
        default:
            break;
    }
}

void KimbleBase::occupy_block_on_board(Player_t *player, uint8_t block_number, uint8_t peg_id, bool finish_lane_flag)
{
    uint8_t count = 0;
    bool done = false;

    if (!finish_lane_flag) {
        board->pos_normal_lane[block_number].occupied = true;
        board->pos_normal_lane[block_number].pos = block_number;
        for (uint8_t i = 0; i < MAX_NUMBER_OF_PEGS; i++) {
            if (board->pos_normal_lane[block_number].occupant[i].peg_id == -1) {
                if (!done){
                    board->pos_normal_lane[block_number].occupant[i].peg_id = peg_id;
                    board->pos_normal_lane[block_number].occupant[i].player_id = player->player_id;
                    done = true;
                    count++;
                }
            } else if (board->pos_normal_lane[block_number].occupant[i].peg_id != -1
                       && (board->pos_normal_lane[block_number].occupant[i].player_id == player->player_id)){
                count++;
            }
        }
        if (count > 1) {
            sprintf(buf, "Multiple Pegs by Player#%s, at POS=%d (Normal Lane)", player->config.player_name, block_number);
            ENGINE_LOG(buf);
        }
    } else {
        board->pos_finish_lane[block_number].occupied = true;
        board->pos_finish_lane[block_number].pos = block_number;
        for (uint8_t i = 0; i < MAX_NUMBER_OF_PEGS; i++) {
            if (board->pos_finish_lane[block_number].occupant[i].peg_id == -1) {
                if (!done) {
                    board->pos_finish_lane[block_number].occupant[i].peg_id = peg_id;
                    board->pos_finish_lane[block_number].occupant[i].player_id = player->player_id;
                    done = true;
                    count++;
                }
            } else if(board->pos_finish_lane[block_number].occupant[i].peg_id != -1
                      && (board->pos_finish_lane[block_number].occupant[i].player_id == player->player_id)){
                count++;
            }
        }
        if (count > 1) {
            sprintf(buf, "Multiple Pegs by Player#%s, at POS=%d (Finish Lane)", player->config.player_name, block_number);
            ENGINE_LOG(buf);
        }
    }

    sprintf(buf, "Player#%s, Peg#%d occupied POS=%d, (%s), DISTANCE_TO_POP=%d", player->config.player_name, peg_id, block_number,
            finish_lane_flag ? "Finish Lane" : "Normal Lane",
            player->meta_data.pegs[peg_id].distance_to_pop);
    ENGINE_LOG(buf);
}

void KimbleBase::free_block_on_board(Player_t *player, uint8_t block_number, uint8_t peg_id, bool finish_lane_flag)
{
    uint8_t count = 0;

    if (!finish_lane_flag) {
        for (uint8_t i = 0; i < MAX_NUMBER_OF_PEGS; i++) {
            if (board->pos_normal_lane[block_number].occupant[i].peg_id != -1
                    && board->pos_normal_lane[block_number].occupant[i].player_id == player->player_id) {
                if (board->pos_normal_lane[block_number].occupant[i].peg_id  == peg_id) {
                    board->pos_normal_lane[block_number].occupant[i].peg_id  = -1;
                    board->pos_normal_lane[block_number].occupant[i].player_id  = -1;
                    board->pos_normal_lane[block_number].pos = block_number;
                } else {
                    count+=1;
                }
            }
        }
        if (count == 0) {
            // If there was only 1 peg there which we just cleared, change the block status to un-occupied
            board->pos_normal_lane[block_number].occupied = false;
        }
    } else {

        for (uint8_t i = 0; i < MAX_NUMBER_OF_PEGS; i++) {
            if (board->pos_finish_lane[block_number].occupant[i].peg_id != -1) {
                if (board->pos_finish_lane[block_number].occupant[i].peg_id == peg_id
                        && board->pos_finish_lane[block_number].occupant[i].player_id == player->player_id) {
                    board->pos_finish_lane[block_number].occupant[i].peg_id = -1;
                    board->pos_finish_lane[block_number].occupant[i].player_id  = -1;
                    board->pos_finish_lane[block_number].pos = block_number;
                } else {
                    count+=1;
                }
            }
        }
        if (count == 0) {
            board->pos_finish_lane[block_number].occupied = false;
        }
    }

    sprintf(buf, "Player#%s vacating POS=%d (%s)", player->config.player_name, block_number,
            finish_lane_flag ? "Finish Lane":"Normal Lane");
    ENGINE_LOG(buf);
}

const char *KimbleBase::id_to_name(uint8_t id)
{
    Player_t *player = NULL;
    assert(players.access_player_data(id, player) == SUCCESS);

    return player->config.player_name;
}

void KimbleBase::turn_toss(uint8_t *player_ids, const uint8_t length)
{
    int *die_outcome=new int[length];
	Turn_sequence_t *turns=new Turn_sequence_t[length];
	//int die_outcome[4];
	//Turn_sequence_t turns[4];

    //roll die for each player and record the result
    for (uint8_t i = 0; i < length; i++) {
        die_outcome[i] = roll_die();
        turns[i].player_id = *player_ids++;
        turns[i].die_outcome = die_outcome[i];
    }

    //sort in descending order
    uint8_t flag = 1;
    uint8_t i, j, player_id, outcome;
    for (i = 0; (i < length) && flag; i++) {
        flag = 0;
        for (j = 0; j < length-1; j++) {
            if (turns[j + 1].die_outcome > turns[j].die_outcome) {
                player_id = turns[j].player_id;
                outcome = turns[j].die_outcome;
                turns[j].player_id = turns[j + 1].player_id;
                turns[j].die_outcome = turns[j + 1].die_outcome;
                turns[j + 1].player_id = player_id;
                turns[j + 1].die_outcome = outcome;
                flag = 1;
            }
        }
    }

    memcpy(turn_seq, turns, sizeof (Turn_sequence_t)*length);

    ENGINE_LOG("\n---- TURN TOSS ----");

    for (uint8_t i = 0; i < length; i++) {
        sprintf(buf, "\t Turn %d -> Player = %s, dice rolled=%d",i, id_to_name(turn_seq[i].player_id), turn_seq[i].die_outcome);
        ENGINE_LOG(buf);
    }
	delete die_outcome;
	delete turns;
}

int8_t KimbleBase::get_next_peg_at_home(uint8_t player_id)
{
    Player_t *player = NULL;
    assert(players.access_player_data(player_id, player) == SUCCESS);

    for (uint8_t peg_id = 0; peg_id < MAX_NUMBER_OF_PEGS; peg_id++) {
        if (player->meta_data.pegs[peg_id].peg_state == IN_HOME) {
            return peg_id;
        }
    }

    return NO_MORE_PEGS_AT_HOME;
}

int8_t KimbleBase::kill_oponents_peg(uint8_t player_id, uint8_t block_pos)
{
    // player whose peg will die, board position will be vacated
    Player_t *player= NULL;
    int8_t peg_id = -1;

    assert(players.access_player_data(player_id, player) == SUCCESS);

    for (uint8_t i=0; i<MAX_NUMBER_OF_PEGS; i++) {
        // There can be multiple pegs of the player on the same spot,
        // kill anyone of them
        if (board->pos_normal_lane[block_pos].occupant[i].peg_id != -1) {
            peg_id = board->pos_normal_lane[block_pos].occupant[i].peg_id ;
            break;
        }
    }

    // make absolutely sure that block position is occupied by a peg of this
    // specific id belonging to this specific player
    assert(peg_id >= 0);
    //assert(player->meta_data.pegs[peg_id].peg_position == block_pos);

    sprintf(buf, "%s\'s PEG#%d will die", player->config.player_name, peg_id);
    ENGINE_LOG(buf);

    free_block_on_board(player, block_pos, (uint8_t)peg_id, false);

    player->meta_data.pegs[peg_id].distance_covered = 0;
    player->meta_data.pegs[peg_id].distance_to_origin_marker = NORMAL_LANE_BLOCKS;
    player->meta_data.pegs[peg_id].distance_to_pop = TOTAL_BLOCKS+1;
    player->meta_data.pegs[peg_id].peg_number = peg_id;
    player->meta_data.pegs[peg_id].peg_position = IN_HOME;
    player->meta_data.pegs[peg_id].peg_state = IN_HOME;

    player->meta_data.pegs_at_home+=1;
    if (player->meta_data.pegs_in_normal_lane >= 0) {
        player->meta_data.pegs_in_normal_lane-=1;
    }

    return SUCCESS;
}

int8_t KimbleBase::move_peg_to_position(Player_t *player, uint8_t peg_id, uint8_t steps, int8_t flag)
{
    if (player->meta_data.pegs_in_normal_lane || player->meta_data.pegs_in_finish_lane || (flag == IN_HOME)) {

        uint8_t zone_start_idx;
        bool lane_transition = false;
        bool finish_lane_flag = false;
        bool pop_up_flag = false;

        switch (player->config.peg_color) {
            case BLUE:
                zone_start_idx = BLUE_ZONE_START;
                break;
            case GREEN:
                zone_start_idx = GREEN_ZONE_START;
                break;
            case RED:
                zone_start_idx = RED_ZONE_START;
                break;
            case YELLOW:
                zone_start_idx = YELLOW_ZONE_START;
                break;
            default:
                return INVALID_PARAMETER;
                break;
        }

        int8_t prev_pos = player->meta_data.pegs[peg_id].peg_position;
        int8_t distance_to_origin = player->meta_data.pegs[peg_id].distance_to_origin_marker;
        int8_t distance_covered = player->meta_data.pegs[peg_id].distance_covered;
        int8_t peg_status = player->meta_data.pegs[peg_id].peg_state;
        uint8_t next_pos;

        Position_ocupant_t occupants_at_next_pos[MAX_NUMBER_OF_PEGS];

        if (peg_status == IN_FINISH_LANE) {
            finish_lane_flag = true;
        } else if (peg_status == POPPED_OUT) {
            sprintf(buf, "Player#%s, Peg#%d has already popped out",player->config.player_name, player->meta_data.pegs[peg_id].peg_number);
            ENGINE_LOG(buf);
            return PEG_ALREADY_POPPED_OUT;
        }

        if ((distance_covered + steps > NORMAL_LANE_BLOCKS)
                && finish_lane_flag == false) {
            finish_lane_flag = true;
            lane_transition = true;
            player->meta_data.pegs[peg_id].peg_state = IN_FINISH_LANE;
            steps -= distance_to_origin;
        }

        // determine the next position of the peg on the board
        if (prev_pos == IN_HOME) {
            next_pos = zone_start_idx;
        } else if ((prev_pos >= 0) && finish_lane_flag) {
            if (lane_transition) {
                next_pos=steps-1; //because next position index in Finish lane starts from 0
            } else {
                next_pos = prev_pos+steps;
            }
        } else {
            next_pos = (prev_pos + steps) % NORMAL_LANE_BLOCKS;
        }

        // fill in the occupants at next position in an array
        for (uint8_t i=0; i<MAX_NUMBER_OF_PEGS; i++ && !finish_lane_flag) {
            occupants_at_next_pos[i] = board->pos_normal_lane[next_pos].occupant[i];
         }

        // if you are in Circulation and a position is occupied, kill opponent
        // if you were in HOME before, and are trying to land on START and START
        // is occupied, you get an OPERATION_NOT_ALLOWED error.

        if (finish_lane_flag == false) {
            if ((next_pos >= zone_start_idx) && flag == IN_CIRCULATION) {
                if (board->pos_normal_lane[next_pos].occupied) {
                    for (uint8_t i = 0; i < MAX_NUMBER_OF_PEGS; i++) {
                        if ((occupants_at_next_pos[i].player_id != player->player_id) && (occupants_at_next_pos[i].player_id != -1)) {
                            // kill the opponent
                            kill_oponents_peg((uint8_t)occupants_at_next_pos[i].player_id, next_pos);
                            sprintf(buf, "Player#%s killed %s's Peg#%d at POS=%d", id_to_name(player->player_id),
                                    id_to_name(occupants_at_next_pos[i].player_id),
                                    occupants_at_next_pos[i].peg_id, next_pos);
                            game_engine_log(&(player->config), buf);
                            break;
                        }
                    }
                }
            } else if ((next_pos == zone_start_idx) && flag == IN_HOME) {
                if (board->pos_normal_lane[next_pos].occupied) {
                    for (uint8_t i = 0; i < MAX_NUMBER_OF_PEGS; i++) {
                        if ((occupants_at_next_pos[i].player_id != player->player_id) && (occupants_at_next_pos[i].player_id != -1)) {
                            sprintf(buf, "OPERATION_NOT_ALLOWED. Player#%s occupies your START",
                                    id_to_name(board->pos_normal_lane[next_pos].occupant[0].player_id ));
                            ENGINE_LOG(buf);
                            return OPERATION_NOT_ALLOWED;
                        }
                    }
                }
            }
        }

        // otherwise proceed normally
        if (player->meta_data.pegs[peg_id].peg_state == IN_HOME) {
            player->meta_data.pegs_at_home-=1;
            player->meta_data.pegs[peg_id].peg_position = zone_start_idx;
            player->meta_data.pegs[peg_id].distance_to_origin_marker -= 1;
            player->meta_data.pegs[peg_id].peg_state = IN_CIRCULATION;
            player->meta_data.pegs[peg_id].distance_covered = 1;
            player->meta_data.pegs[peg_id].distance_to_pop -= 1;
            player->meta_data.pegs_in_normal_lane += 1;

        } else if (player->meta_data.pegs[peg_id].peg_state == IN_CIRCULATION) {

            player->meta_data.pegs[peg_id].peg_position = next_pos;
            player->meta_data.pegs[peg_id].distance_covered += steps;
            player->meta_data.pegs[peg_id].distance_to_origin_marker -= steps;
            player->meta_data.pegs[peg_id].distance_to_pop -= steps;

        } else if (player->meta_data.pegs[peg_id].peg_state == IN_FINISH_LANE) {

            if (next_pos < FINISH_POST_MARKER) {
                // move inside finish lane
                if (lane_transition) {
                    player->meta_data.pegs_in_finish_lane += 1;
                    if (player->meta_data.pegs_in_normal_lane >= 0) {
                        player->meta_data.pegs_in_normal_lane -= 1;
                    }
                }
                player->meta_data.pegs[peg_id].distance_covered = steps;
                player->meta_data.pegs[peg_id].distance_to_pop = FINISH_POST_MARKER-next_pos;
                player->meta_data.pegs[peg_id].distance_to_origin_marker = -1;
                player->meta_data.pegs[peg_id].peg_position = next_pos;

            } else if (next_pos == FINISH_POST_MARKER) {
                // congrats, this peg can pop out now
                player->meta_data.pegs[peg_id].distance_covered = next_pos+NORMAL_LANE_BLOCKS;
                player->meta_data.pegs[peg_id].distance_to_pop = 0;
                player->meta_data.pegs[peg_id].distance_to_origin_marker = -1;
                player->meta_data.pegs[peg_id].peg_position = 33;
                player->meta_data.pegs[peg_id].peg_state = POPPED_OUT;
                // update peg-wide stats
                if (player->meta_data.pegs_in_finish_lane >= 0) {
                    player->meta_data.pegs_in_finish_lane -= 1;
                }
                player->meta_data.pegs_popped_out += 1;
                if ( player->meta_data.pegs_in_play >= 0) {
                    player->meta_data.pegs_in_play -= 1;
                }
				if(player->meta_data.pegs_popped_out == MAX_NUMBER_OF_PEGS) {
                     remaining_players_on_board-=1;
                 }
                pop_up_flag = true;
            } else {
                sprintf(buf, "Player#%s, Peg#%d Cannot pop. OPERATION_NOT_ALLOWED", player->config.player_name, peg_id);
                return OPERATION_NOT_ALLOWED;
            }
        }

        // occupy new block on board if the peg has not popped out
        if (player->meta_data.pegs[peg_id].peg_state != POPPED_OUT) {
            occupy_block_on_board(player, next_pos, peg_id, finish_lane_flag);
        }

        // free previous block from board
        // If previous location was IN_HOME, do not free
        if (prev_pos != IN_HOME) {
            // if a transit from IN_CIRCULATION to IN_FINISH_LANE happened
            // free previous occupation in normal lane
            free_block_on_board(player, prev_pos, peg_id, lane_transition ? IN_CIRCULATION : finish_lane_flag);
        }

        //Log the stuff
        if (pop_up_flag) {
            sprintf(buf, "Peg#%d Popped Out. PEGS_AT_HOME=%d, PEGS_IN_LANE=%d, PEGS_IN_FINISH_LANE=%d, PEGS_POPPED_OUT=%d", peg_id,
                    player->meta_data.pegs_at_home, player->meta_data.pegs_in_normal_lane,
                    player->meta_data.pegs_in_finish_lane, player->meta_data.pegs_popped_out);

            game_engine_log(&(player->config), buf);
        } else {
            sprintf(buf, "Peg#%d MOVED to POS=%d (%s)", peg_id, player->meta_data.pegs[peg_id].peg_position,
                    finish_lane_flag ? "Finish Lane" : "Normal Lane");
            game_engine_log(&(player->config), buf);
        }

        return SUCCESS;
    }

    sprintf(buf, "Player#%s, NO_PEGS_IN_PLAY", player->config.player_name);
    ENGINE_LOG(buf);
    return NO_PEGS_IN_PLAY;
}

void state_machine_messag(int8_t reason, const char *msg)
{
    switch (reason) {
        case PLAYER_DOES_NOT_EXIST:
            sprintf(buf, "%s - PLAYER_DOES_NOT_EXIST, %d\n", msg, reason);
            break;
        case INVALID_PARAMETER:
            sprintf(buf, "%s - INVALID_PARAMETER, %d\n", msg, reason);
            break;
        case PLAYER_ALREADY_EXIST:
            sprintf(buf, "%s - PLAYER_ALREADY_EXIST, %d\n", msg, reason);
            break;
        case OPERATION_NOT_ALLOWED:
            sprintf(buf, "%s - OPERATION_NOT_ALLOWED, %d\n", msg, reason);
            break;
        case PEG_ALREADY_POPPED_OUT:
            sprintf(buf, "%s - PEG_ALREADY_POPPED_OUT, %d\n", msg, reason);
            break;
        case NO_MORE_PEGS_AT_HOME:
            sprintf(buf, "%s - NO_MORE_PEGS_AT_HOME, %d\n", msg, reason);
            break;
        case NO_PEGS_IN_PLAY:
            sprintf(buf, "%s - NO_MORE_PEGS_IN_PLAY, %d\n", msg, reason);
            break;
    }

    LOG("[SM_MSG]: %s", buf);
}

int8_t KimbleBase::get_out_of_home(uint8_t player_id)
{
    int8_t retcode;
    Player_t *player_data = NULL;

    players.access_player_data(player_id, player_data);
    retcode = get_next_peg_at_home(player_id);


    if (retcode < 0) {
        state_machine_messag(retcode, "Can't move a Peg out of HOME");
        return retcode;
    }

    sprintf(buf, "Attempt -> MOVE peg#%d from HOME to START", retcode);
    game_engine_log(&(player_data->config), buf);
    retcode = move_peg_to_position(player_data, (uint8_t)retcode, 0, IN_HOME);

    return retcode;

}


static void peg_to_pop_dist_order(Player_t *player, int8_t *ordered_list)
{
    //sort in descending order
    uint8_t flag = 1;
    uint8_t i, j;

    Peg_t pegs[4];
    Peg_t temp_pegs;

    memcpy(pegs, player->meta_data.pegs, sizeof(pegs));

    for (i = 0; (i < MAX_NUMBER_OF_PEGS) && flag; i++) {
        flag = 0;
        for (j = 0; j < MAX_NUMBER_OF_PEGS-1; j++) {
            if ((pegs[j + 1].distance_to_pop > pegs[j].distance_to_pop) && (pegs[j].distance_to_pop != 0)) {
                temp_pegs = pegs[j];
                pegs[j] = pegs[j + 1];
                pegs[j + 1] = temp_pegs;
                flag = 1;
            }
        }
    }

    for (i = 0; i<MAX_NUMBER_OF_PEGS; i++) {
        ordered_list[i] = pegs[i].peg_number;
    }
}

void KimbleBase::set_player_status(Player_t *player)
{
    uint8_t winner_count = 0;
    uint8_t still_playing_count = 0;

	Player_summary_t *summary=new Player_summary_t[number_of_players];

    players.player_status_summary(summary);

    for (uint8_t i = 0; i < number_of_players; i++) {
        if (summary[i].config.peg_color == player->config.peg_color) {
            continue;
        }
        if (summary[i].player_status ==AWAITNG_TURN
                || summary[i].player_status == ROLLING) {
            still_playing_count++;
        } else if (summary[i].player_status == WON) {
            winner_count++;
        }
    }

    if (still_playing_count == 0) {
        player->meta_data.player_status = LOST;
        player->meta_data.player_standing = winner_count+1;
    } else if (still_playing_count > 0 ) {
        if (player->meta_data.pegs_popped_out == MAX_NUMBER_OF_PEGS) {
            if ( player->meta_data.player_status != WON) {
                player->meta_data.player_status = WON;
                player->meta_data.player_standing = winner_count+1;
            }
        } else {
            player->meta_data.player_status =AWAITNG_TURN;
        }
		delete summary;
    }
	
}

int8_t KimbleBase::game_engine()
{
    int8_t retcode;
    bool rolling = false;
    int8_t list_peg_ids[MAX_NUMBER_OF_PEGS];

    ENGINE_LOG("\t---- SIMULATING GAME ----\n");

    uint8_t status =AWAITNG_TURN;
    while (status != LOST) {
        // round-robin pop-o-matic push
        for (uint8_t i = 0; i < number_of_players; i++) {
            //current user data before rolling the die
            Player_t *player = NULL;
            int8_t counter  = MAX_NUMBER_OF_PEGS-1;
            uint8_t peg_id;

            retcode = players.access_player_data(turn_seq[i].player_id, player);

        if (retcode == SUCCESS && (player->meta_data.player_status == AWAITNG_TURN)) {
			if (retcode == SUCCESS && (player->meta_data.player_status == AWAITNG_TURN)
                                   && (remaining_players_on_board > 1)) {

                rolling = true;

                while (rolling) {
                    player->meta_data.player_status = ROLLING;
                    turn_seq[i].die_outcome = roll_die();
                    sprintf(buf, "Rolled dice ... %d", turn_seq[i].die_outcome);
                    game_engine_log(&(player->config), buf);

                    if (turn_seq[i].die_outcome == 6) {
                        // Try to get a peg out from HOME if possible
                        if (player->meta_data.pegs_at_home > 0) {
                            if (get_out_of_home(player->player_id) == SUCCESS) {
                                // success in getting a peg out
                                // get_out_of_home() have already moved the peg to START
                                // so continue with another roll
                                continue;
                            }
                        }

                        // If there was no Peg at home or START is occupied, get_out_of_home()
                        // will fail, in which case we try to move a peg available on the
                        // normal or finish lane if possible
                        if (player->meta_data.pegs_in_normal_lane > 0 || player->meta_data.pegs_in_finish_lane > 0) {
                            peg_to_pop_dist_order(player, list_peg_ids);
                            counter = MAX_NUMBER_OF_PEGS-1;
                            while (counter>=0) {
                                peg_id = list_peg_ids[counter];
                                if (move_peg_to_position(player, peg_id, turn_seq[i].die_outcome,
                                                         player->meta_data.pegs[peg_id].peg_state) == SUCCESS){
                                    break;
                                }

                                counter--;
                            }

                            player->meta_data.player_status =AWAITNG_TURN;
                        }
                        // get another turn as you had popped 6
                        continue;
                    } else {
                        rolling = false;
                        if (player->meta_data.pegs_in_normal_lane > 0 || player->meta_data.pegs_in_finish_lane > 0) {
                            // as a basic strategy, move the peg nearest to Finish lane
                            peg_to_pop_dist_order(player, list_peg_ids);
                            counter = MAX_NUMBER_OF_PEGS-1;
                            while (counter>=0) {
                                peg_id = list_peg_ids[counter];
                                if (move_peg_to_position(player, (uint8_t)peg_id, turn_seq[i].die_outcome,
                                                         player->meta_data.pegs[peg_id].peg_state) == SUCCESS){
                                    break;
                                }
                                counter--;
                            }

                            player->meta_data.player_status =AWAITNG_TURN;
                        }
                    }
                }
            }
		}
		

            // set player status at the end of each turn
            set_player_status(player);
            if (player->meta_data.player_status == LOST) {
                status = LOST;
				break;
            }
        } //end of "for (uint8_t i = 0; i < number_of_players; i++)"
    } // end of while(status != LOST)

    compile_results();

    return SUCCESS;
}

void KimbleBase::compile_results()
{
    Player_t *player = NULL;
    Player_config_t player_config[MAX_NUMBER_OF_PLAYERS];

    printf("\n *********** RESULTS ********* \n");

    for (uint8_t i = 0; i < number_of_players; i++) {
        players.access_player_data(i, player);
        uint8_t temp = player->meta_data.player_standing;
        for (uint8_t j = 1; j <= 4; j++) {
            if (temp == j) {
                player_config[j-1] = player->config;
            }
        }
    }

    game_engine_log(&(player_config[0]), " -> Winner !! .. Congratulations");
    game_engine_log(&(player_config[1]), " -> Runners Up");
    game_engine_log(&(player_config[2]), " -> 3rd Position");
    game_engine_log(&(player_config[3]), " -> Lost ! .. Better luck next time");
}

int8_t KimbleBase::simulate_game(Player_config_t *config)
{
    int8_t retcode;
    uint8_t player_ids[4] = {0,1,2,3};
    if (number_of_players > 4 || number_of_players < 1) {
        return INVALID_PARAMETER;
    }

    retcode = players.create_players(number_of_players, config);
    assert(retcode == SUCCESS);

    turn_toss(player_ids, 4);

    game_engine();


    return retcode;
}
