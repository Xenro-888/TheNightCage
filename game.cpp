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

void place_start_tiles(board& this_board, std::vector<player> players)
{
	std::string player_input;

	for (player curr_player : players)
	{
		std::shared_ptr<tile> plr_start_tile = std::make_shared<tile>(tile(start_tile));
		int tile_y = -1;
		int tile_x = -1;

		while (!this_board.valid_index(tile_x, tile_y))
		{
			std::cout << "WHERE WOULD YOU LIKE TO PLACE YOUR START TILE?" << std::endl;
			std::getline(std::cin, player_input);
			if (player_input.length() == 3)
			{
				tile_x = player_input[0] - 49;
				tile_y = player_input[2] - 49;
			}
		}
		this_board.place_tile(*plr_start_tile, tile_x, tile_y);
		this_board.place_player(curr_player, tile_x, tile_y);
		//this_board.illuminate(curr_player);
		this_board.display();
	}
}

void player_turn(player& current_player, board& this_board)
{
	std::string player_input;

	display_player_turn(current_player);
	while (player_input != "MOVE" && player_input != "STAY")
	{
		std::cout << "WHAT WOULD YOU LIKE TO DO?\n";
		std::cout << "MOVE OR STAY\n";
		std::getline(std::cin, player_input);

		if (player_input == "MOVE")
		{
			// get valid corridor input
			int corridor = -1;
			while (corridor < 0 || corridor > 5)
			{
				std::cout << "WHICH CORRIDOR WOULD YOU LIKE TO GO DOWN?\n";
				std::cout << "1: UP | 2: DOWN | 3: LEFT | 4: RIGHT\n";
				std::getline(std::cin, player_input);

				if (player_input.length() == 1 && std::isdigit(player_input[0]))
					corridor = std::stoi(player_input);
			}

			this_board.move_player(current_player, corridor);
		}
		else if (player_input == "STAY")
		{
			
		}
	}
	this_board.display();
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
	place_start_tiles(this_board, this_board.players);

	// main game loop
	while (!game_over)
	{
		for (player current_player : this_board.players)
		{
			std::shared_ptr<tile> current_tile = this_board.play_area[current_player.get_y()][current_player.get_x()];
			player_turn(current_player, this_board);
		}
	}

	this_board.~board();
}