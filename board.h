#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include <map>
#include "tile.h"
#include "player.h"

using std::array, std::shared_ptr, std::vector, std::map;

class board
{
	map<tile_type, int> tile_type_counts;

public:
	array<array<shared_ptr<tile>, 6>, 6> play_area;
	vector<player> players;

	bool valid_index(int x, int y);
	void place_tile(shared_ptr<tile> tile_to_place, int x, int y);
	bool place_player(player& player_to_place, int x, int y);

	shared_ptr<tile> get_standing_tile(player& prisoner);
	shared_ptr<tile> get_adj_tile(int x, int y, int corridor);
	array<array<int, 2>, 4> get_corridor_directions();
	map<tile_type, int> get_tile_type_counts();

	void set_tile_type_count(tile_type type_to_set, int number);

	shared_ptr<tile> new_random_tile(vector<tile_type> allowed_types);

	bool move_player(player& player_to_move, int corridor);
	void illuminate(player& lit_player);
	void darkness();

	void move_tile(shared_ptr<tile> tile_to_move, int x, int y);
	void destroy_tile(shared_ptr<tile> tile_to_destroy);
	void display();

	board();
	~board();
};

#endif /* BOARD_H */
