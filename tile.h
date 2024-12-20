#ifndef TILE_H
#define TILE_H

#include <array>
#include <memory>
#include "player.h"

using std::unique_ptr;

enum tile_type {unspecified, straight_tile, t_tile, cross_tile, key_tile, gate_tile, start_tile, pit_tile, wax_eater};

int modulo(int num, int div);

class tile
{
	int x;
	int y;
	std::array<bool, 4> corridors;
	tile_type type;
	player* standing_player;

public:

	int get_x();
	int get_y();
	std::array<bool, 4> get_corridors();
	tile_type get_type();
	int is_cracked();
	player* get_standing_player();

	void set_x(int x);
	void set_y(int y);
	void set_standing_player(player* standing_player);

	void rotate();

	tile(tile_type type);
};

#endif /* TILE_H */
