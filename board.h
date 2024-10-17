#pragma once
#include <vector>
#include "tile.h"
#include "player.h"

class board
{
public:
	tile* play_area[6][6];
	std::vector<player*> players;

	bool valid_index(int x, int y);
	void place_tile(tile* tile_to_place, int x, int y);
	bool place_player(player* player_to_place, int x, int y);

	tile* get_standing_tile(player* prisoner);
	tile* get_adj_tile(int x, int y, int corridor);

	bool move_player(player* player_to_move, int corridor);
	void illuminate(player* lit_player);
	void darkness();

	void move_tile(tile* tile_to_move, int x, int y);
	void destroy_tile(tile* tile_to_destroy);
	void display();

	board();
	~board();
};

