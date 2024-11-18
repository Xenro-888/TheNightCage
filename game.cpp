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
	
	std::cout << "\x1b[" << 3 * 6 << ";0f\n\n";
	std::cout << "\x1b[2K";
	if (player_color == 0)
		std::cout << GREEN << "GREEN'S TURN" << std::endl;
	else if (player_color == 1)
		std::cout << YELLOW << "YELLOW'S TURN" << std::endl;
	else if (player_color == 2)
		std::cout << PURPLE << "PURPLE'S TURN" << std::endl;
	else if (player_color == 3)  
		std::cout << BLUE << "BLUE'S TURN" << std::endl;

	std::cout << "\x1b[97m";
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
				tile_x = player_input[0] - 49;
				tile_y = player_input[2] - 49;
			}
		}
		this_board.place_tile(plr_start_tile, tile_x, tile_y);
		this_board.place_player(curr_player, tile_x, tile_y);
		this_board.illuminate(curr_player);
		this_board.display();
	}
}

void player_is_moving(player& player_to_move, board& this_board)
{
	std::string player_input;
	int selected_corridor = -1;
	bool can_move = false;

	while (!can_move || player_input.length() != 1 || !std::isdigit(player_input[0]))
	{
		std::cout << "WHICH CORRIDOR DO YOU FOLLOW?\n";
		std::cout << "UP: 1 | LEFT: 2 | DOWN: 3 | RIGHT : 4\n";
		std::getline(std::cin, player_input);

		selected_corridor = std::stoi(player_input);

		can_move = this_board.move_player(player_to_move, selected_corridor);	
		if (player_to_move.is_lit())
			this_board.illuminate(player_to_move);

		this_board.darkness();
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
			player_is_moving(current_player, this_board);
			this_board.darkness();
		}
	}

	this_board.~board();
}