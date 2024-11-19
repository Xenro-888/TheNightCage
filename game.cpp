#include <iostream>
#include <string>
#include <memory>
#include "board.h"

void display_player_turn(player& current_player)
{
	const std::string GREEN = "\x1b[32m";
	const std::string YELLOW = "\x1b[33m";
	const std::string PURPLE = "\x1b[35m";
	const std::string BLUE = "\x1b[34m";
	int player_color = current_player.get_color();
	
	std::cout << "\x1b[" << 3 * 6 << ";0f\n\n"; // set cursor position to under the displayed board
	std::cout << "\x1b[2K"; // clear the line the cursor is on
	if (player_color == 0)
		std::cout << GREEN << "GREEN'S TURN" << std::endl;
	else if (player_color == 1)
		std::cout << YELLOW << "YELLOW'S TURN" << std::endl;
	else if (player_color == 2)
		std::cout << PURPLE << "PURPLE'S TURN" << std::endl;
	else if (player_color == 3)  
		std::cout << BLUE << "BLUE'S TURN" << std::endl;

	std::cout << "\x1b[97m"; // set cursor color to white
}

void place_start_tiles(board& this_board, std::vector<player>& players)
{
	std::string player_input;

	for (player& curr_player : players)
	{
		std::shared_ptr<tile> plr_start_tile = std::make_shared<tile>(tile(start_tile));
		int tile_y = -1;
		int tile_x = -1;

		while (!this_board.valid_index(tile_x, tile_y))
		{
			std::cout << "WHERE WOULD YOU LIKE TO PLACE YOUR START TILE?\n";
			std::cout << "(X COORDINATE, Y COORDINATE)\n";
			std::getline(std::cin, player_input);
			if (player_input.length() == 3)
			{
				// turn the player input to numeric coordinates
				tile_x = player_input[0] - '0' - 1;
				tile_y = player_input[2] - '0' - 1;
			}
		}

		this_board.place_tile(plr_start_tile, tile_x, tile_y);
		this_board.place_player(curr_player, tile_x, tile_y);
		this_board.illuminate(curr_player);
		this_board.display();
	}
}

/*
move_player()

Move the player down the corridor they specify.

Args:
	player_to_move (player): The player who wants to move

Returns:
	None.
*/
void move_player(player& player_to_move, board& this_board)
{
	std::string player_input;
	int selected_corridor = -1;
	bool can_move = false;

	std::cout << "WHICH CORRIDOR DO YOU WANT TO WALK DOWN?\n";
	std::cout << "(UP, DOWN, LEFT, RIGHT)\n";

	while (!can_move)
	{
		// gain input
		while (player_input != "UP" && player_input != "DOWN" && player_input != "LEFT" && player_input != "RIGHT")
		{
			std::getline(std::cin, player_input);
		}

		// turn the word into the numeric corridor representation
		if (player_input == "UP")
			selected_corridor = 0;
		else if (player_input == "LEFT")
			selected_corridor = 1;
		else if (player_input == "DOWN")
			selected_corridor = 2;
		else if (player_input == "RIGHT")
			selected_corridor = 3;

		// perform player movement
		can_move = this_board.move_player(player_to_move, selected_corridor);
		if (player_to_move.is_lit())
			this_board.illuminate(player_to_move);

		// tell the player if they did something wrong
		if (!can_move)
			std::cout << "YOU CAN'T MOVE THERE.\n";

		player_input.clear();
	}
}

void start_game()
{
	board this_board = board();
	bool game_over = false;

	// init players
	for (int i = 0; i < 4; i++)
	{
		this_board.players.push_back(player(i));
	}

	// start of game
	place_start_tiles(this_board, this_board.players);

	// main game loop
	while (!game_over)
	{
		for (player& current_player : this_board.players)
		{
			display_player_turn(current_player);
			move_player(current_player, this_board);
			this_board.darkness();
			this_board.display();
		}
	}

	this_board.~board();
}