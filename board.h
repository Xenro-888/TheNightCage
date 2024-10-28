#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include "tile.h"
#include "player.h"

class board
{
public:
	std::shared_ptr<tile> play_area[6][6];
	std::vector<player> players;

	bool valid_index(int x, int y);
	void place_tile(tile& tile_to_place, int x, int y);
	bool place_player(player& player_to_place, int x, int y);

	std::shared_ptr<tile> get_standing_tile(player& prisoner);
	std::shared_ptr<tile> get_adj_tile(int x, int y, int corridor);
	std::array<std::array<int, 2>, 4> get_corridor_directions();

	bool move_player(player& player_to_move, int corridor);
	void illuminate(player& lit_player);
	void darkness();

	void move_tile(std::shared_ptr<tile> tile_to_move, int x, int y);
	void destroy_tile(tile& tile_to_destroy);
	void display();

	board();
	~board();
};

#endif /* BOARD_H */
