#include "StdAfx.h"
#include "Player.h"
#include "Utilities.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

Player::Player()
{
    player_list_head = NULL;
}

Player::~Player()
{
    drop_list(player_list_head);
}

int8_t Player::access_player_data(uint8_t player_id, Player_t *&player_data_ptr)
{
     Player_entry_t *current = player_list_head;
     Player_entry_t *next;

     while (current != NULL) {

         next = current->next;

         if (current->player_data->player_id == player_id) {
             player_data_ptr = current->player_data;
             return SUCCESS;
         }

         current = next;
     }

     return PLAYER_DOES_NOT_EXIST;
}

int8_t Player::create_players(uint8_t num_players, Player_config_t *config)
{
    Player_t player;
    int8_t retcode;

    for (uint8_t i=0; i < num_players; i++) {
        player.player_id = config->peg_color; //player ID's are tied to peg color i.e, RED player will have ID 0
        player.config = *config;
        config++;
        player.meta_data.pegs_in_play = MAX_NUMBER_OF_PEGS;
        player.meta_data.player_standing = 0;
        player.meta_data.pegs_popped_out = 0;
        player.meta_data.pegs_in_normal_lane = 0;
        player.meta_data.pegs_in_finish_lane = 0;
        player.meta_data.player_status = AWAITNG_TURN;
        for (uint8_t peg_num=0; peg_num < MAX_NUMBER_OF_PEGS; peg_num++) {
            player.meta_data.pegs[peg_num].peg_number= peg_num;
            player.meta_data.pegs[peg_num].peg_state= IN_HOME;
            player.meta_data.pegs[peg_num].peg_position = IN_HOME;
            player.meta_data.pegs[peg_num].distance_covered = 0;
            player.meta_data.pegs[peg_num].distance_to_pop = 32+1; // 32 block position and 1 arbitrary pop-out position
            player.meta_data.pegs[peg_num].distance_to_origin_marker = 28; //28 Normal Lane blocks, 28th is the origin marker and then 4 Finish lane blocks
        }

        //ith player is ready to be integrated into the list
        player.meta_data.pegs_at_home = MAX_NUMBER_OF_PEGS;
        retcode = add_entry_to_list(player_list_head, &player);
    }
    return retcode;
}

void Player::player_status_summary(Player_summary_t *summary)
{
    Player_entry_t *current = player_list_head;
    Player_entry_t *next;

    while (current != NULL) {

        next = current->next;

        summary->config = current->player_data->config;
        summary->player_status = current->player_data->meta_data.player_status;

        summary++;

        current = next;
    }
}

// Enter a player to the players list
int8_t Player::add_entry_to_list(Player_entry_t *&list_head, Player_t *data)
{
    Player_t *new_player = new Player_t;
    memcpy(new_player, data, sizeof(Player_t));
    Player_entry_t *entry = new Player_entry_t;
    entry->player_data = new_player;
    if (list_head == NULL) {
        //Means its a first entry, next pointer is set to null
        entry->next = NULL;
        list_head = entry;
    } else {
        //Means we need to add new entry as new Head and the previous Head gets pushed back
        entry->next = list_head;
        list_head = entry;
    }

    return SUCCESS;
}

// Drop the whole list
int8_t Player::drop_list(Player_entry_t *&list_head)
{
    Player_entry_t *current = list_head;
    Player_entry_t *next;

    while (current != NULL) {
        //extract next pointer
        next = current->next;
        //delete current entry
        delete current->player_data;
        delete current;

        current = next;
    }

    list_head = NULL;

    return SUCCESS;

}