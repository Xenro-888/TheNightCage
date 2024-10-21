#include <iostream>
#include <vector>
#include <algorithm>
#include "board.h"

bool board::valid_index(int x, int y)
{
	if ((x < 0 || x > 5) || (y < 0 || y > 5))
		return false;

	return true;
}

void board::place_tile(tile* tile_to_place, int x, int y)
{
	if ((x < 0 || x > 5) || (y < 0 || y > 5))
		return;

	if (play_area[y][x] != nullptr)
		return;

	play_area[y][x] = tile_to_place;
	tile_to_place->set_x(x);
	tile_to_place->set_y(y);
}

bool board::place_player(player* player_to_place, int x, int y)
{
	if (!valid_index(x, y))
		return false;

	tile* spot = play_area[y][x];
	if (spot->get_standing_player() != nullptr)
		return false;

	spot->set_standing_player(player_to_place);
	player_to_place->set_x(x);
	player_to_place->set_y(y);
	return true;
}

tile* board::get_standing_tile(player* prisoner)
{
	return play_area[prisoner->get_y()][prisoner->get_x()];
}

tile* board::get_adj_tile(int x, int y, int corridor)
{
	int corridor_directions[][2] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };
	tile* adj_tile = play_area
		[modulo(y + corridor_directions[corridor][1], 6)]
		[modulo(x + corridor_directions[corridor][0], 6)];

	return adj_tile;
}

bool board::move_player(player* player_to_move, int corridor) 
{
	tile* first_tile = play_area[player_to_move->get_y()][player_to_move->get_x()];
	tile* next_tile = get_adj_tile(first_tile->get_x(), first_tile->get_y(), corridor);

	if (
		!first_tile->get_corridors()[corridor] ||
		!next_tile->get_corridors()[corridor + 2 % 4] ||
		next_tile->get_standing_player() != nullptr
		)
	{
		return false;
	}

	first_tile->set_standing_player(nullptr);
	player_to_move->set_x(next_tile->get_x());
	player_to_move->set_y(next_tile->get_y());

	if (next_tile->get_type() != pit_tile)
		next_tile->set_standing_player(player_to_move);

	if (player_to_move->is_lit())
		illuminate(player_to_move);
	darkness();
	display();

	// activate monsters
	return true;
}

void board::illuminate(player* lit_player)
{
	if (lit_player->is_falling())
		return;

	tile* standing_tile = get_standing_tile(lit_player);
	int corridor_directions[][2] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };

	for (int corridor = 0; corridor < 4; corridor++)
	{
		int* curr_corr_direction = corridor_directions[corridor];
		int new_tile_type = -1;
		int new_tile_x = (lit_player->get_x() + curr_corr_direction[0]) % 6;
		int new_tile_y = (lit_player->get_y() + curr_corr_direction[1]) % 6;
		tile* adj_tile = play_area[new_tile_y][new_tile_x];

		if (!standing_tile->get_corridors()[corridor] || adj_tile != nullptr)
			continue;

		while (new_tile_type < 0 || new_tile_type == 6)
		{
			std::cout << "TILE TYPE LOOP!\n";
			new_tile_type = rand() % 8 + 1;
		}

		tile* new_tile = new tile((tile_type)new_tile_type);
		std::array<bool, 4> tile_corridors = new_tile->get_corridors();
		std::cout << new_tile_x << ", " << new_tile_y << '\n';
		place_tile(new_tile, new_tile_x, new_tile_y);

		while (!tile_corridors[(corridor + 2) % 6])
		{
			std::cout << "MAKING SURE NEXT TILE CONNECTS LOOP!\n";
			new_tile->rotate();
		}
	}
}

void board::darkness()
{
	std::vector<tile*> safe_tiles;
	int corridor_directions[][2] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };

	for (player* curr_player : players)
	{
		tile* standing_tile = get_standing_tile(curr_player);
		int player_x = curr_player->get_x();
		int player_y = curr_player->get_y();

		if (!curr_player->is_lit() || curr_player->is_falling())
			continue;

		safe_tiles.push_back(standing_tile);
		for (int corridor = 0; corridor < 4; corridor++)
		{
			if (!standing_tile->get_corridors()[corridor])
				continue;

			tile* adj_tile = get_adj_tile(player_x, player_y, corridor);
			safe_tiles.push_back(adj_tile);
		}
	}

	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			tile* curr_tile = play_area[y][x];
			if (curr_tile == nullptr || std::find(safe_tiles.begin(), safe_tiles.end(), curr_tile) != safe_tiles.end())
				continue;
			destroy_tile(curr_tile);
		}
	}
}

void board::move_tile(tile* tile_to_move, int x, int y)
{
	if (play_area[y][x] != nullptr)
		return;

	int prev_x = tile_to_move->get_x();
	int prev_y = tile_to_move->get_y();
	play_area[prev_y][prev_x] = nullptr;
	play_area[y][x] = tile_to_move;
	tile_to_move->set_x(x);
	tile_to_move->set_y(y);
}

void board::destroy_tile(tile* tile_to_destroy)
{
	play_area[tile_to_destroy->get_y()][tile_to_destroy->get_x()] = nullptr;
	delete tile_to_destroy;
}

void board::display()
{
	std::cout << "DISPLAYING!\n";
	const char CROSS_PIPE = (char)206;
	const char VERTICAL_PIPE = (char)186;
	const char HORIZONTAL_PIPE = (char)205;

	std::cout << "\x1b[1J";
	for (player* curr_player : players)
	{
		int x = curr_player->get_x();
		int y = curr_player->get_y();
		if (x != -1 && y != -1)
			continue;

		if (x == -1)
			std::cout << "\x1b[" << y * 3 + 3 << ";" << 6 * 3 + 1 << "H";
		else if (y == -1)
			std::cout << "\x1b[0;" << x * 3 + 2 << "H";
		std::cout << "*";
	}
	std::cout << "DISPLAYED FALLING PLAYERS!\n";

	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			tile* current_tile = play_area[y][x];
			std::array<bool, 4> corridors = current_tile->get_corridors();
			tile_type curr_tile_type = current_tile->get_type();
			player* standing_player = current_tile->get_standing_player();

			int curr_tile_console_x = current_tile->get_x() * 3 + 2;
			int curr_tile_console_y = current_tile->get_y() * 3 + 3;

			if (current_tile == nullptr)
				continue;

			// move cursor to tile position
			std::cout << "\x1b[" <<
				curr_tile_console_y << ";" <<
				curr_tile_console_x << "f";

			if (standing_player != nullptr)
			{
				int color = standing_player->get_color();

				if (color == 0)
					std::cout << "\x1b[32m";
				else if (color == 1)
					std::cout << "\x1b[33m";
				else if (color == 2)
					std::cout << "\x1b[35m";
				else if (color == 3)
					std::cout << "\x1b[34m";
			}
			else if (curr_tile_type == pit_tile)
				std::cout << "\x1b[90m";
			else if (current_tile->is_cracked())
			{
				//std::cout << "\x1b[91m";
			}

			// draw center of tile
			if (curr_tile_type == pit_tile)
				std::cout << (char)178;
			else if (curr_tile_type == straight_tile)
				std::cout << (corridors[0] ? VERTICAL_PIPE : HORIZONTAL_PIPE);
			else if (curr_tile_type == start_tile)
			{
				if (corridors[0] && corridors[1])
					std::cout << (char)188;
				else if (corridors[1] && corridors[2])
					std::cout << (char)187;
				else if (corridors[2] && corridors[3])
					std::cout << (char)201;
				else if (corridors[3] && corridors[4])
					std::cout << (char)200;
			}
			else if (curr_tile_type == t_tile)
			{
				if (corridors[0] && corridors[1] && corridors[2])
					std::cout << (char)185;
				else if (corridors[1] && corridors[2] && corridors[3])
					std::cout << (char)203;
				else if (corridors[2] && corridors[3] && corridors[0])
					std::cout << (char)204;
				else if (corridors[3] && corridors[1] && corridors[0])
					std::cout << (char)202;
			}
			else if (curr_tile_type == wax_eater)
				std::cout << "X";
			else
				std::cout << CROSS_PIPE;

			/*
			std::cout << "\x1b[38;5;241m"; // set color to white
			// draw corridors
			for (int i = 0; i < 4; i++)
			{
				int corridor = corridors[i];
				int corridor_directions[][2] = {
					{0, 1},
					{-1, 0},
					{0, -1},
					{1, 0}
				};

				std::cout << "\x1b[" <<
					curr_tile_console_y - corridor_directions[i][1] << ";" <<
					curr_tile_console_x + corridor_directions[i][0] << "f";

				if (corridor)
				{
					if (i % 2 == 0)
						std::cout << VERTICAL_PIPE;
					else
						std::cout << HORIZONTAL_PIPE;
				}
			}
			*/
			std::cout << "\x1b[38;5;248m";
		}
		std::cout << std::endl;
	}
	std::cout << "\x1b[" << 3 * 6 + 1 << ";0f" << std::endl;
	std::cout << "  1  2  3  4  5  6  " << std::endl;
}

board::board()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			play_area[i][j] = nullptr;
		}
	}
}

board::~board()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			tile* tile = play_area[i][j];
			if (tile == nullptr)
				continue;

			delete play_area[i][j];
		}
	}
}