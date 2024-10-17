#include <iostream>
#include <string>
#include <conio.h>
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
		int x = 0;
		int y = 0;
		int last_x = 0;
		int last_y = 0;

		// make sure the start tile isn't over an already existing tile
		for (y; y < y + 6; y = modulo(y + 1, 6))
		{
			for (x; x < x + 6; x = modulo(x + 1, 6))
			{
				if (this_board.play_area[y][x] == nullptr)
					goto place_tile;
			}
		}

	place_tile:
		this_board.place_tile(plr_start_tile, x, y);
		this_board.display();

		// player input
		int key = 0;
		while (key != 13)
		{
			int x_dir = 0;
			int y_dir = 0;
			key = _getch();

			if (key == 72)
			{
				y = modulo(y - 1, 6);
				y_dir = 1;
			}
			else if (key == 75)
			{
				x = modulo(x - 1, 6);
				x_dir = -1;
			}
			else if (key == 80)
			{
				y = modulo(y + 1, 6);
				y_dir = -1;
			}
			else if (key == 77)
			{
				x = modulo(x + 1, 6);
				x_dir = 1;
			}
			else if (key == 114)
			{
				plr_start_tile->rotate();
				this_board.display();
			}

			if (x != last_x || y != last_y)
			{
				this_board.move_tile(plr_start_tile, x, y);
				this_board.display();
			}
			last_x = x;
			last_y = y;
		}
		this_board.place_player(curr_player, x, y);
		this_board.illuminate(curr_player);
		this_board.display();
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
				std::getline(std::cin, player_input);
			}

			if (player_input == "M")
			{
				bool moved = false;
				while (!moved) {
					int corridor = -1;
					std::cout << "USE THE ARROW KEYS TO ENTER THE CORRIDOR YOU'D LIKE TO GO DOWN.\n";

					while (corridor < 0)
					{
						int key = _getch();
						if (key == 72)
							corridor = 0;
						else if (key == 75)
							corridor = 1;
						else if (key == 80)
							corridor = 2;
						else if (key == 77)
							corridor = 3;
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