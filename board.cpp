#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <cmath>
#include "board.h"

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

	std::cout << "PLACED TILE\n";
}

bool board::place_player(player& player_to_place, int x, int y)
{
	if (!valid_index(x, y))
		return false;

	shared_ptr<tile> spot = play_area[y][x];
	if (spot->get_standing_player() != nullptr)
		return false;

	spot->set_standing_player(&player_to_place);
	player_to_place.set_x(x);
	player_to_place.set_y(y);
	return true;
}

shared_ptr<tile> board::get_standing_tile(player& prisoner)
{
	return play_area[prisoner.get_y()][prisoner.get_x()];
}

shared_ptr<tile> board::get_adj_tile(int x, int y, int corridor)
{
	array<array<int, 2>, 4> corridor_directions = get_corridor_directions();
	shared_ptr<tile> adj_tile = play_area
		[modulo(y + corridor_directions[corridor][1], 6)]
		[modulo(x + corridor_directions[corridor][0], 6)];

	return adj_tile;
}

array<array<int, 2>, 4> board::get_corridor_directions()
{
	array<array<int, 2>, 4> corridor_directions;
	corridor_directions[0] = {0, -1};
	corridor_directions[1] = {-1, 0};
	corridor_directions[2] = {0, 1};
	corridor_directions[3] = {1, 0};

	return corridor_directions;
}

map<tile_type, int> board::get_tile_type_counts() { return tile_type_counts; }

void board::set_tile_type_count(tile_type type_to_set, int number) { tile_type_counts[type_to_set] = number; }

shared_ptr<tile> board::new_random_tile(vector<tile_type> allowed_types)
{
	tile_type new_tile_type = unspecified;
	srand(time(NULL)); // set the seed to random

	while (
		(std::find(allowed_types.begin(), allowed_types.end(), new_tile_type) == allowed_types.end()) || 
		(new_tile_type == unspecified) ||
		tile_type_counts[new_tile_type] <= 0
	)
	{
		new_tile_type = (tile_type) (rand() % 9);
	}

	shared_ptr<tile> new_tile = std::make_shared<tile>(tile(new_tile_type));
	tile_type_counts[new_tile_type] -= 1; // decrement the count for this type of tile
	return new_tile;
}

bool board::move_player(player& player_to_move, int corridor) 
{
	shared_ptr<tile> first_tile = play_area[player_to_move.get_y()][player_to_move.get_x()];
	shared_ptr<tile> next_tile = get_adj_tile(first_tile->get_x(), first_tile->get_y(), corridor);
	array<bool, 4> first_tile_corridors = first_tile.get()->get_corridors();
	array<bool, 4> next_tile_corridors = next_tile.get()->get_corridors();

	// if there is no corridor on the current tile, or there's no corridor connecting the next tile to the current one
	if (!first_tile_corridors[corridor] || !next_tile_corridors[(corridor + 2) % 4])
		return false;

	// if there is already a player on the next tile
	if (next_tile.get()->get_standing_player() != nullptr && next_tile.get()->get_type() != gate_tile)
		return false;

	// if the player is moving onto a pit
	if (next_tile.get()->get_type() == pit_tile)
	{
		first_tile.get()->set_standing_player(nullptr);

		std::cout << "YOU FELL AND LOST A TURN!\n";
		std::cout << "WOULD YOU LIKE TO FALL ONTO A TILE THAT IS ON YOUR CURRENT ROW OR COLUMN?\n";
		std::string player_input;

		// gain player input
		while (player_input != "ROW" && player_input != "COLUMN")
		{
			std::getline(std::cin, player_input);
		}

		if (player_input == "ROW")
			player_to_move.set_y(-1);
		else
			player_to_move.set_x(-1);

		player_to_move.set_fall_state(true);
		return true;
	}
	// if the player's candle isn't lit and there isn't already a set next tile to move onto
	else if (!player_to_move.is_lit())
	{
		return true;
	}

	first_tile.get()->set_standing_player(nullptr);
	next_tile.get()->set_standing_player(&player_to_move);
	player_to_move.set_x(next_tile.get()->get_x());
	player_to_move.set_y(next_tile.get()->get_y());

	return true;
}

void board::illuminate(player& lit_player)
{
	if (lit_player.is_falling())
		return;

	std::cout << "START\n";
	array<bool, 4> current_tile_corridors = get_standing_tile(lit_player).get()->get_corridors();
	array<array<int, 2>, 4> corridor_directions = get_corridor_directions();

	std::cout << "GOT BASIC CORRIDOR INFO\n";
	
	// iterate through each corridor and illuminate accordingly
	for (int corridor = 0; corridor < 4; corridor++)
	{
		std::cout << "CORRIDOR: " << corridor << "\n";
		if (!current_tile_corridors[corridor])
			continue;	

		array<int, 2> current_corridor_direction = corridor_directions[corridor];
		int adjacent_spot_x = modulo(lit_player.get_x() + current_corridor_direction[0], 6);
		int adjacent_spot_y = modulo(lit_player.get_y() + current_corridor_direction[1], 6);

		std::cout << "ADJ POS: " << adjacent_spot_x << ", " << adjacent_spot_y << "\n";
		shared_ptr<tile> adjacent_spot = play_area[adjacent_spot_y][adjacent_spot_x];

		std::cout << "GOT ADJ SPOT\n";

		if (adjacent_spot.get() != nullptr)
			continue;

		// choose the new tile's type
		tile_type new_tile_type = unspecified;
		srand(time(NULL)); // set the seed to random
		while (new_tile_type == unspecified || new_tile_type == start_tile)
		{
			new_tile_type = (tile_type) (rand() % 9);
		}
		new_tile_type = pit_tile;
		shared_ptr<tile> new_tile = std::make_shared<tile>(tile(new_tile_type));

		std::cout << "NEW TILE AND TYPE SELECTED\n";

		// rotate the tile to make sure it connects to the current tile
		array<bool, 4> new_tile_corridors = new_tile.get()->get_corridors();
		while (!new_tile_corridors[(corridor + 2) % 4])
		{
			new_tile.get()->rotate();
			new_tile_corridors = new_tile.get()->get_corridors();
		}

		std::cout << "ABOUT TO PLACE TILE\n";

		place_tile(new_tile, adjacent_spot_x, adjacent_spot_y);
	}
}

void board::darkness()
{
	vector<shared_ptr<tile>> safe_tiles;

	// find the tiles that are surely illuminated and add them to vector
	for (player& curr_player : players)
	{
		if (curr_player.is_falling())
			continue;

		shared_ptr<tile> standing_tile = get_standing_tile(curr_player);
		int player_x = curr_player.get_x();
		int player_y = curr_player.get_y();

		if (standing_tile == nullptr || !curr_player.is_lit() || curr_player.is_falling())
			continue;

		safe_tiles.push_back(standing_tile);
		for (int corridor = 0; corridor < 4; corridor++)
		{
			if (!standing_tile->get_corridors()[corridor])
				continue;

			shared_ptr<tile> adjacent_tile = get_adj_tile(player_x, player_y, corridor);
			safe_tiles.push_back(adjacent_tile);
		}
	}
 
	// destroy tiles that aren't in the safe tile vector
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			shared_ptr<tile> current_tile = play_area[y][x];
			if (current_tile == nullptr)
				continue;

			auto found_safe_tile = std::find_if(
				safe_tiles.begin(), 
				safe_tiles.end(), 
				[&current_tile](const shared_ptr<tile> safe_tile) { return safe_tile.get() == current_tile.get(); }
			);
	
			if (found_safe_tile == safe_tiles.end())
			{
				destroy_tile(current_tile);
			}
		}
	}
}
void board::move_tile(shared_ptr<tile> tile_to_move, int x, int y)
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

	// display player falling tokens
	for (player& current_player : players)
	{
		if (!current_player.is_falling())
			continue;

		int cursor_x_position = -1;
		int cursor_y_position = -1;
		
		if (current_player.get_x() == -1)
		{
			cursor_x_position = 0;
			cursor_y_position = current_player.get_y() * TILE_SIZE + 3;
		}
		else if (current_player.get_y() == -1)
		{
			cursor_x_position = current_player.get_x() * TILE_SIZE + 3;
			cursor_y_position = 0;
		}

		// print player color
		int player_color = current_player.get_color();
		if (player_color == 0)
			std::cout << "\x1b[32m";
		else if (player_color == 1)
			std::cout << "\x1b[33m";
		else if (player_color == 2)
			std::cout << "\x1b[35m"
;		else if (player_color == 3)  
			std::cout << "\x1b[34m";

		std::cout << "\x1b[" << cursor_y_position << ";" << cursor_x_position << "H"; // set cursor position to token position
		std::cout << "*";
		std::cout << "\x1b[37m"; // reset color to white
	}

	std::cout << "\x1b[0;0H"; // reset cursor position
	// display tiles
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			shared_ptr<tile> current_tile = play_area[y][x];
			if (current_tile.get() == nullptr)
				continue;
				
			tile_type current_tile_type = current_tile->get_type();
			int tile_console_x = current_tile->get_x() * TILE_SIZE + 3;
			int tile_console_y = current_tile->get_y() * TILE_SIZE + 3;

			std::cout << "\x1b[" << tile_console_y << ";" << tile_console_x << "H"; // set cursor position to the tile's console position

			// display center of tile
			player* standing_player = current_tile.get()->get_standing_player();
			if (standing_player != nullptr)
			{
				int player_color = standing_player->get_color();
				if (player_color == 0)
					std::cout << "\x1b[32m";
				else if (player_color == 1)
					std::cout << "\x1b[33m";
				else if (player_color == 2)
					std::cout << "\x1b[35m";
				else if (player_color == 3)
					std::cout << "\x1b[34m";
			}
			if (current_tile_type == pit_tile)
				std::cout << "0";
			else
				std::cout << "+";

			// display tile corridors
			array<array<int, 2>, 4> corridor_directions = get_corridor_directions();
			std::cout << "\x1b[37m"; // reset cursor color back to white
			for (int corridor = 0; corridor < 4; corridor++)
			{
				array<int, 2>& current_corridor_direction = corridor_directions[corridor];
				if (!current_tile.get()->get_corridors()[corridor])
					continue;

				// set cursor position to the corridor's position
				std::cout << "\x1b[" << 
					tile_console_y + current_corridor_direction[1] << ";" << 
					tile_console_x + current_corridor_direction[0] << "H";

				if (corridor % 2 == 0)
					std::cout << "|";
				else
					std::cout << "-";
			}
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

	// init tile counts
	tile_type_counts[straight_tile] = 10;
	tile_type_counts[cross_tile] = 12;
	tile_type_counts[t_tile] = 30;
	tile_type_counts[wax_eater] = 12;
	tile_type_counts[start_tile] = 4;
	tile_type_counts[key_tile] = 6;
	tile_type_counts[gate_tile] = 4;
}

// defenition isn't needed currently as shared pointers will be deallocated at the end of the program
board::~board()
{
	
}