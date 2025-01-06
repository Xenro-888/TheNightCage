#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "tile.h"
#include "player.h"

using std::array, std::shared_ptr, std::vector, std::map;

const std::string GREEN = "\x1b[32m";
const std::string YELLOW = "\x1b[33m";
const std::string PURPLE = "\x1b[35m";
const std::string BLUE = "\x1b[34m";
const int TILE_SIZE = 3;

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
	void run_for_each_tile(std::function<void(shared_ptr<tile> current_tile)>);

	void set_tile_type_count(tile_type type_to_set, int number);
	void rotate_until_conected(shared_ptr<tile> anchor_tile, shared_ptr<tile> tile_to_rotate);

	shared_ptr<tile> new_random_tile(vector<tile_type> allowed_types);
	bool line_of_sight(shared_ptr<tile> tile1, shared_ptr<tile> tile2);

	bool move_player(player& player_to_move, int corridor);
	void illuminate(player& lit_player);
	void darkness();
	void activate_monster(shared_ptr<tile> monster_tile);

	void move_tile(shared_ptr<tile> tile_to_move, int x, int y);
	void destroy_tile(shared_ptr<tile> tile_to_destroy);
	void display();
	void clear_under_display();

	board();
	~board();
};

#endif /* BOARD_H */
