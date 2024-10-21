#include <iostream>
#include <string>
//#include <conio.h>
#include "board.h"

void display_plr_turn(player* curr_player)
{
	const std::string GREEN = "\x1b[32m";
	const std::string YELLOW = "\x1b[33m";
	const std::string PURPLE = "\x1b[35m";
	const std::string BLUE = "\x1b[34m";
	
	std::cout << "\x1b[" << 3 * 6 << ";0f\n\n";
	std::cout << "\x1b[2K";
	if (curr_player->get_color() == 0)
		std::cout << GREEN << "GREEN'S TURN" << std::endl;
	else if (curr_player->get_color() == 1)
		std::cout << YELLOW << "YELLOW'S TURN" << std::endl;
	else if (curr_player->get_color() == 2)
		std::cout << PURPLE << "PURPLE'S TURN" << std::endl;
	else if (curr_player->get_color() == 3)  
		std::cout << BLUE << "BLUE'S TURN" << std::endl;
	std::cout << "\x1b[97m";
}

void game_start(board &this_board, std::vector<player*> players)
{
	std::string player_input;

	for (player* curr_player : players)
	{
		tile* plr_start_tile = new tile(start_tile);
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
			//std::cout << tile_x << ", " << tile_y << std::endl;
		}
		std::cout << "VALID INDEX!" << std::endl;
		this_board.place_tile(plr_start_tile, tile_x, tile_y);
		this_board.place_player(curr_player, tile_x, tile_y);
		this_board.illuminate(curr_player);
		this_board.display();

		std::cout << plr_start_tile->get_x() << ", " << plr_start_tile->get_y() << std::endl;
		std::cin.ignore();
	}
}

void start_game()
{
	board this_board = board();
	bool game_over = false;

	for (int i = 0; i < 4; i++)
		this_board.players.push_back(new player(i));
	game_start(this_board, this_board.players);

	while (!game_over)
	{
		for (player* curr_player : this_board.players)
		{
			std::string player_input;
			tile* standing_tile = this_board.play_area[curr_player->get_y()][curr_player->get_x()];

			display_plr_turn(curr_player);
			while (player_input != "M" && player_input != "S")
			{
				std::cout << "WHAT WOULD YOU LIKE TO DO?\nM: MOVE | S: STAY\n";
				std::cin >> player_input;
				std::getline(std::cin, player_input);
				std::cin.ignore();
			}
			player_input.clear();

			if (player_input == "M")
			{
				bool moved = false;
				while (!moved) {
					std::cout << "WHICH CORRIDOR WOULD YOU LIKE TO GO DOWN?" << std::endl;
					std::cout << "UP: 0 | LEFT: 1 | DOWN: 2 | RIGHT: 3" << std::endl;
					std::getline(std::cin, player_input);

					int corridor = -1;
					while (corridor < 0 || corridor > 3)
					{
						corridor = std::stoi(player_input);
					}
					moved = this_board.move_player(curr_player, corridor);
				}
				player_input.clear();

				tile* standing_tile = this_board.get_standing_tile(curr_player);
				if (standing_tile->get_type() == pit_tile)
				{
					standing_tile->set_standing_player(nullptr);
					std::cout << "YOU'VE FELL DOWN A PIT! WOULD YOU LIKE TO STAY ON YOUR ROW (R) OR COLUMN (C)?" << std::endl;
					std::getline(std::cin, player_input);
					if (player_input == "R")
						curr_player->set_x(-1);
					if (player_input == "C")
						curr_player->set_y(-1);
					this_board.darkness(); // called again after in board::move player because the player isn't standing on any tiles anymore
				}
				this_board.display();
			}
			else if (player_input == "S")
			{

			}
		}
	}

	this_board.~board();
}