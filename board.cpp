#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "board.h"

using std::array, std::shared_ptr, std::vector;

int modulo(int number, int divisor)
{
	int result = number % divisor;
	if (result < 0)
		result += divisor;

	return result;
}

bool board::valid_index(int x, int y)
{
	if ((x < 0 || x > 5) || (y < 0 || y > 5))
		return false;

	return true;
}

void board::place_tile(shared_ptr<tile> tile_to_place, int x, int y)
{
	play_area[y][x] = tile_to_place;
	tile_to_place->set_x(x);
	tile_to_place->set_y(y);
}

bool board::place_player(player& player_to_place, int x, int y)
{
	if (!valid_index(x, y))
		return false;

	std::shared_ptr<tile> spot = play_area[y][x];
	if (spot->get_standing_player() != nullptr)
		return false;

	spot->set_standing_player(&player_to_place);
	player_to_place.set_x(x);
	player_to_place.set_y(y);
	return true;
}

std::shared_ptr<tile> board::get_standing_tile(player& prisoner)
{
	return play_area[prisoner.get_y()][prisoner.get_x()];
}

std::shared_ptr<tile> board::get_adj_tile(int x, int y, int corridor)
{
	std::array<std::array<int, 2>, 4> corridor_directions = get_corridor_directions();
	std::shared_ptr<tile> adj_tile = play_area
		[modulo(y + corridor_directions[corridor][1], 6)]
		[modulo(x + corridor_directions[corridor][0], 6)];

	return adj_tile;
}

std::array<std::array<int, 2>, 4> board::get_corridor_directions()
{
	std::array<std::array<int, 2>, 4> corridor_directions;
	corridor_directions[0] = {0, -1};
	corridor_directions[1] = {-1, 0};
	corridor_directions[2] = {0, 1};
	corridor_directions[3] = {1, 0};

	return corridor_directions;
}

bool board::move_player(player& player_to_move, int corridor) 
{
	std::shared_ptr<tile> first_tile = play_area[player_to_move.get_y()][player_to_move.get_x()];
	std::shared_ptr<tile> next_tile = get_adj_tile(first_tile->get_x(), first_tile->get_y(), corridor);

	if (
		!first_tile->get_corridors()[corridor] ||
		!next_tile->get_corridors()[corridor + 2 % 4] ||
		next_tile->get_standing_player() != nullptr
		)
	{
		return false;   
	}

	first_tile->set_standing_player(nullptr);
	player_to_move.set_x(next_tile->get_x());
	player_to_move.set_y(next_tile->get_y());

	if (next_tile->get_type() != pit_tile)
		next_tile->set_standing_player(&player_to_move);

	if (player_to_move.is_lit())
		illuminate(player_to_move);
	darkness();
	display();

	// activate monsters
	return true;
}

void board::illuminate(player& lit_player)
{
	std::shared_ptr<tile> current_standing_tile = get_standing_tile(lit_player);
	std::array<bool, 4> current_tile_corridors = current_standing_tile->get_corridors();
	std::array<std::array<int, 2>, 4> corridor_directions = get_corridor_directions();
	
	for (int corridor = 0; corridor < 4; corridor++)
	{
		std::array<int, 2> current_corridor_direction = corridor_directions[corridor];
		int adjacent_spot_x = modulo(lit_player.get_x() + current_corridor_direction[0], 6);
		int adjacent_spot_y = modulo(lit_player.get_y() + current_corridor_direction[1], 6);

		if (!current_tile_corridors[corridor] || play_area[adjacent_spot_y][adjacent_spot_x] != nullptr)
			continue;

		// select new tile type
		tile_type new_tile_type = (tile_type)0;
		while (new_tile_type == unspecified || new_tile_type == start_tile)
		{
			new_tile_type = (tile_type)(rand() % 8);
		}

		// create and place new tile
		shared_ptr<tile> new_tile = std::make_shared<tile>(tile(new_tile_type));
		place_tile(new_tile, adjacent_spot_x, adjacent_spot_y);
	}
}

void board::darkness()
{
	vector<shared_ptr<tile>> safe_tiles;
	int corridor_directions[][2] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };

	for (player& curr_player : players)
	{
		shared_ptr<tile> standing_tile = get_standing_tile(curr_player);
		int player_x = curr_player.get_x();
		int player_y = curr_player.get_y();

		if (!curr_player.is_lit() || curr_player.is_falling())
			continue;

		safe_tiles.push_back(standing_tile);
		for (int corridor = 0; corridor < 4; corridor++)
		{
			if (!standing_tile->get_corridors()[corridor])
				continue;

			shared_ptr<tile> adj_tile = get_adj_tile(player_x, player_y, corridor);
			safe_tiles.push_back(adj_tile);
		}
	}

	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			std::shared_ptr<tile> curr_tile = play_area[y][x];
			auto found = std::find_if(
				safe_tiles.begin(), safe_tiles.end(), 
				[&curr_tile](const shared_ptr<tile> tile) { return tile.get() == curr_tile.get(); }
			);
	
			if (curr_tile == nullptr || found != safe_tiles.end())
				continue;
				
			destroy_tile(curr_tile);
		}
	}
}

void board::move_tile(std::shared_ptr<tile> tile_to_move, int x, int y)
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

void board::destroy_tile(shared_ptr<tile> tile_to_destroy)
{
	play_area[tile_to_destroy->get_y()][tile_to_destroy->get_x()] = nullptr;
}

void board::display()
{
	const int TILE_SIZE = 3;
	std::cout << "| THE NIGHT CAGE |\n";
	std::cout << "\x1b[2J"; // clear screen

	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			std::shared_ptr<tile> current_tile = play_area[y][x];
			if (current_tile.get() == nullptr)
				continue;
				
			tile_type current_tile_type = current_tile->get_type();
			int tile_console_x = current_tile->get_x() * TILE_SIZE + 1;
			int tile_console_y = current_tile->get_y() * TILE_SIZE + 1;
			std::cout << "\x1b[" << tile_console_y << ";" << tile_console_x << "H"; // set cursor position to the tile's console position

			// display center of tile
			if (current_tile_type == pit_tile)
				std::cout << "0";
			else
				std::cout << "+";

			// display tile corridors
		}
	}

	// move cursor to under the displayed board
	std::cout << "\x1b[" << TILE_SIZE * 6 + TILE_SIZE - 2 << ";" << TILE_SIZE * 6 + TILE_SIZE - 2 << "H";
}

board::board()
{
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			play_area[y][x] = nullptr;
		}
	}
}

// defenition isn't needed currently as shared pointers will be deallocated at the end of the program
board::~board()
{
	
}