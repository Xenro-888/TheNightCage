#include <iostream>
#include <array>
#include "tile.h"

int tile::get_x() { return x; }
int tile::get_y() { return y; }
std::array<bool, 4>& tile::get_corridors() { return corridors; }
tile_type tile::get_type() { return type; }
player* tile::get_standing_player() { return standing_player; }

void tile::set_x(int x) { this->x = x; }
void tile::set_y(int y) { this->y = y; }
void tile::set_standing_player(player* standing_player) { this->standing_player = standing_player; }

void tile::rotate()
{
	int first_corridor = corridors[0];
	for (int i = 0; i < 3; i++)
	{
		corridors[i] = corridors[modulo(i + 1, 4)];
	}
	corridors[3] = first_corridor;
}

int tile::is_cracked()
{
	if (type == straight_tile || type == key_tile)
		return true;

	return false;
}

tile::tile(tile_type type)
{
	if (type == unspecified)
		type = straight_tile;

	standing_player = nullptr;
	x = 0;
	y = 0;
	this->type = type;

	if (type == straight_tile)
	{
		corridors[0] = 1;
		corridors[1] = 0;
		corridors[2] = 1;
		corridors[3] = 0;
	}
	else if (type == t_tile)
	{
		corridors[0] = 1;
		corridors[1] = 1;
		corridors[2] = 1;
		corridors[3] = 0;
	}
	else if (type == cross_tile || type == gate_tile || type == pit_tile || type == key_tile || type == wax_eater)
	{
		for (int i = 0; i < 4; i++)
			corridors[i] = 1;
	}
	else if (type == start_tile)
	{
		corridors[0] = 1;
		corridors[1] = 1;
		corridors[2] = 0;
		corridors[3] = 0;
	}
}
