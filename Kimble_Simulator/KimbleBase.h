#pragma once
#ifndef INCLUDES_KIMBLEBASE_H_
#define INCLUDES_KIMBLEBASE_H_

#include "Player.h"

#define DIE_FACES                      6 //faces 1-6
#define NORMAL_LANE_BLOCKS             28
#define FINISH_LANE_BLOCKS             4
#define TOTAL_BLOCKS                   NORMAL_LANE_BLOCKS+FINISH_LANE_BLOCKS
#define FINISH_POST_MARKER             4 //steps 0-3 and a final pop-up step
//#define CIRCLE_WRAP_AROUND_MARKER      27 // steps

#define BLUE_ZONE_ORIGIN        0
#define GREEN_ZONE_ORIGIN       7
#define RED_ZONE_ORIGIN         14
#define YELLOW_ZONE_ORIGIN      21

#define BLUE_ZONE_START        BLUE_ZONE_ORIGIN+1
#define GREEN_ZONE_START       GREEN_ZONE_ORIGIN+1
#define RED_ZONE_START         RED_ZONE_ORIGIN+1
#define YELLOW_ZONE_START      YELLOW_ZONE_ORIGIN+1



#define ENGINE_LOG_ENABLED

#ifdef ENGINE_LOG_ENABLED
#define ENGINE_LOG(X)  game_engine_log((NULL), (X));
#else
#define ENGINE_LOG(X)
#endif

typedef struct {
    int8_t player_id;
    int8_t peg_id;
}Position_ocupant_t;

typedef struct {
    bool occupied;         //boolean to mark if the position is occupied by a peg
    bool in_finish_lane;             //boolean to mark if the position is occupied by a peg
    int8_t pos;            //position on the circle with respect to origin, normal lane
    Position_ocupant_t occupant[4];
    //int8_t player_id;      //id of the player who occupied the spot, -1 if unoccupied
    //int8_t peg_ids[MAX_NUMBER_OF_PEGS];         //id of the peg who occupied the spot, -1 if unoccupied
} Position_t;

typedef struct {
    uint8_t origin;                  //origin position in regard to color
    Position_t pos_normal_lane[NORMAL_LANE_BLOCKS];
    Position_t pos_finish_lane[FINISH_LANE_BLOCKS];       //finish lane position indices
} Ground_t;

typedef struct {
    uint8_t player_id;
    uint8_t die_outcome;
} Turn_sequence_t;

class KimbleBase
{
public:
    KimbleBase(uint8_t num_players);
    virtual ~KimbleBase();

    int8_t simulate_game(Player_config_t *config);

    friend class Test;
    Player players; //KimbleBase has Player

private:
    uint8_t number_of_players;
    Ground_t *board;
    Turn_sequence_t *turn_seq;
    void turn_toss(uint8_t *player_ids, const uint8_t length);
    void setup_board(Ground_t *board);
    int8_t game_engine();
    void compile_results();
    void game_engine_log(Player_config_t *config, const char *msg);
    int8_t get_out_of_home(uint8_t player_id);
    int8_t get_next_peg_at_home(uint8_t player_id);
    int8_t kill_oponents_peg(uint8_t player_id, uint8_t block_pos);
    int8_t move_peg_to_position(Player_t *player, uint8_t peg_id, uint8_t step, int8_t flag);
    const char *id_to_name(uint8_t id);
    void free_block_on_board(Player_t *player, uint8_t block_number, uint8_t peg_id, bool finish_lane_flag);
    void occupy_block_on_board(Player_t *player, uint8_t block_number, uint8_t peg_id, bool finish_lane_flag);
    void set_player_status(Player_t *player);
};


#endif /* INCLUDES_KIMBLEBASE_H_ */