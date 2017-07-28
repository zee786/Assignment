// Kimble_Simulator.cpp : Defines the entry point for the console application.
//
/* A Kimble games also called trouble game is a famous board game. It is quite similar to another board game called Ludo. I played a lot of Ludo in Pakistan
and also in Finland. The main reference of the following code is also comes from the Ludo game. */



#include "stdafx.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <conio.h>


#include "KimbleBase.h"
#include "Test.h"
#include "Utilities.h"
using namespace std;

#define     MENU_OUT(X)                 cout << ANSI_COLOR_CYAN X ANSI_COLOR_RESET;

/**
 * Tests
 *
 * Function prototypes for running tests
 */
static void TEST_occupy_block_on_board();
static void TEST_free_block_on_board();
static void TEST_kill_opponent_peg();
static void TEST_enter_finish_lane();
static void TEST_pop_a_peg();
static void TEST_board_single_move_test();

/**
 * Helper Function prototypes
 */
static void do_config_players(Player_config_t *config);
static void simulate_game();
static void run_tests();


int main()
{
    int choice;
    bool gameOn = true;
    while (gameOn != false) {
        MENU_OUT("\n*******************************\n\n");
        MENU_OUT(" 1 - Simulate Game.\n");
        MENU_OUT(" 2 - Play Game [Not supported yet].\n");
        MENU_OUT(" 3 - Run Tests.\n");
        MENU_OUT(" 4 - Exit.\n\n");
        MENU_OUT(" Enter your choice and press return: ");

        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Simulating game...!\n";
                simulate_game();
                gameOn = false;
                break;
            case 2:
                cout << "This feature is not supported yet\n";
                gameOn = false;
                break;
            case 3:
                cout << "Running Tests ...\n";
                run_tests();
                gameOn = false;
                break;
            case 4:
                cout << "Exiting ...\n";
                gameOn = false;
                break;
            default:
                cout << "Not a Valid Choice. \n";
                cout << "Choose again.\n";
                cin >> choice;
                break;
        }
    }

	getch();
    return 0;
}


static void simulate_game()
{
    KimbleBase game(MAX_NUMBER_OF_PLAYERS);
    Player_config_t config[MAX_NUMBER_OF_PLAYERS];
    do_config_players(config);
    game.simulate_game(config);
}

static void do_config_players(Player_config_t *config)
{
    config[0].player_name = "Salman";
    config[0].peg_color = RED;
    config[1].player_name = "Momina";
    config[1].peg_color = GREEN;
    config[2].player_name = "Zeeshan";
    config[2].peg_color = BLUE;
    config[3].player_name = "Ahad";
    config[3].peg_color = YELLOW;
}

static void run_tests()
{
    TEST_occupy_block_on_board();
    TEST_free_block_on_board();
    TEST_kill_opponent_peg();
    TEST_enter_finish_lane();
    TEST_pop_a_peg();
     TEST_board_single_move_test();
}

static void TEST_board_single_move_test()
{
    Test test_env;
    Player_config_t config[MAX_NUMBER_OF_PLAYERS];

    do_config_players(config);

    KimbleBase game(MAX_NUMBER_OF_PLAYERS);
    game.players.create_players(MAX_NUMBER_OF_PLAYERS, config);

    if(test_env.board_single_move_test(&game) != SUCCESS) {
        fprintf(stderr, "Test - board_single_move_test Failed.\n");
    }

    printf("Test - board_single_move_test Passed.\n");
}
static void TEST_enter_finish_lane()
{
    Test test_env;
    Player_config_t config[MAX_NUMBER_OF_PLAYERS];

    do_config_players(config);

    KimbleBase game(MAX_NUMBER_OF_PLAYERS);
    game.players.create_players(MAX_NUMBER_OF_PLAYERS, config);

    if(test_env.enter_finish_lane(&game) != SUCCESS) {
        fprintf(stderr, "Test - enter_finish_lane Failed.\n");
    }

    printf("Test - enter_finish_lane Passed.\n");

}

static void TEST_pop_a_peg()
{
    Test test_env;
    Player_config_t config[MAX_NUMBER_OF_PLAYERS];

    do_config_players(config);

    KimbleBase game(MAX_NUMBER_OF_PLAYERS);
    game.players.create_players(MAX_NUMBER_OF_PLAYERS, config);

    if(test_env.pop_a_peg(&game) != SUCCESS) {
        fprintf(stderr, "Test - pop_a_peg Failed.\n");
    }

    printf("Test - pop_a_peg Passed.\n");

}
static void TEST_kill_opponent_peg()
{
    Test test_env;
    Player_config_t config[MAX_NUMBER_OF_PLAYERS];

    do_config_players(config);

    KimbleBase game(MAX_NUMBER_OF_PLAYERS);
    game.players.create_players(MAX_NUMBER_OF_PLAYERS, config);

    if(test_env.kill_opponent_peg(&game) != SUCCESS) {
        fprintf(stderr, "Test - kill_opponent_peg Failed.\n");
    }

    printf("Test - kill_opponent_peg Passed.\n");
}

static void TEST_occupy_block_on_board()
{
    Test test_env;
    Player_config_t config[MAX_NUMBER_OF_PLAYERS];

    do_config_players(config);

    KimbleBase game(MAX_NUMBER_OF_PLAYERS);
    game.players.create_players(MAX_NUMBER_OF_PLAYERS, config);

    if(test_env.occupy_block_on_board(&game) != SUCCESS) {
        fprintf(stderr, "Test - occupy_block_on_board Failed.\n");
    }

    printf("Test - occupy_block_on_board Passed.\n");
}

static void TEST_free_block_on_board()
{
    Test test_env;
    Player_config_t config[MAX_NUMBER_OF_PLAYERS];

    do_config_players(config);

    KimbleBase game(MAX_NUMBER_OF_PLAYERS);
    game.players.create_players(MAX_NUMBER_OF_PLAYERS, config);

    if(test_env.free_block_on_board(&game) != SUCCESS) {
        fprintf(stderr, "Test - free_block_on_board Failed.\n");
    }

    printf("Test - free_block_on_board Passed.\n");
}