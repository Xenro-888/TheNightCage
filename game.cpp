#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <limits>
#include <csignal>
#include "board.h"
void display_player_turn(player& current_player)
{
	const std::string GREEN = "\x1b[32m";
	const std::string YELLOW = "\x1b[33m";
	const std::string PURPLE = "\x1b[35m";
	const std::string BLUE = "\x1b[34m";
	int player_color = current_player.get_color();
	
	std::cout << "\x1b[" << 3 * 6 << ";0f\n\n"; // set cursor position to under the displayed board
	std::cout << "\x1b[2K"; // clear the line the cursor is on
	if (player_color == 0)
		std::cout << GREEN << "GREEN'S TURN" << std::endl;
	else if (player_color == 1)
		std::cout << YELLOW << "YELLOW'S TURN" << std::endl;
	else if (player_color == 2)
		std::cout << PURPLE << "PURPLE'S TURN" << std::endl;
	else if (player_color == 3)  
		std::cout << BLUE << "BLUE'S TURN" << std::endl;

	std::cout << "\x1b[97m"; // set cursor color to white
}

// get_valid_input()
// Repeatedly attemps to gain valid user input using the ruleset provided.
//
// Arguments:
// ruleset (std::function<bool(std::string)>): a function/lambda that takes the player's input and tests it against a ruleset
std::string get_valid_input(std::function<bool(const std::string)> ruleset)
{
	std::string player_input = "";
	bool valid_input = false;

	while (!valid_input)
	{
		std::getline(std::cin, player_input);

		// check if input is valid
		valid_input = ruleset(player_input);

		if (!valid_input)
		{
			player_input.clear();
			std::cout << "INVALID INPUT. TRY AGAIN.\n";
		}
	}
	
	return player_input;
}

// place_start_tiles()
// Has each player place their start tile in their specified position.
//
// Arguments:
// this_board (board&) the game board
void place_start_tiles(board& this_board)
{
	for (player& current_player : this_board.players) 
	{
		
		bool valid_input = false;
		shared_ptr<tile> player_start_tile = std::make_unique<tile>(start_tile);
		std::function<bool(const std::string)> input_ruleset = [](const std::string input) {
				if (input.length() == 3 && std::isdigit(input[0]) && std::isdigit(input[2]))
					return true;

				return false;
			};

		// get player input
		std::cout << "WHERE WOULD YOU LIKE TO PLACE YOUR STARTING TILE?\n";
		std::cout << "\"X Y\" (STARTING FROM THE TOP-RIGHT CORNER.)\n";
		std::string player_input = get_valid_input(input_ruleset);
		std::cout << "PLAYER_INPUT: " << player_input << "\n"; 

		int tile_x = player_input[0] - '0' - 1;
		int tile_y = player_input[2] - '0' - 1;

		this_board.place_tile(player_start_tile, tile_x, tile_y);
		this_board.place_player(current_player, tile_x, tile_y);
		this_board.illuminate(current_player);
		this_board.display();
	}
}

// move_player()
// Moves the player down the corridor they specify.
void move_player(player& player_to_move, board& this_board)
{
	int selected_corridor = -1;
	bool can_move = false;

	std::cout << "WHICH CORRIDOR DO YOU WANT TO WALK DOWN?\n";
	std::cout << "(UP, DOWN, LEFT, RIGHT)\n";

	while (!can_move)
	{
		// get player input
		std::string player_input = get_valid_input([](const std::string input) -> bool 
			{
				if (input == "UP" || input == "DOWN" || input == "LEFT" || input == "RIGHT")
					return true;

				return false;
			});

		// turn the word into the numeric corridor representation
		if (player_input == "UP")
			selected_corridor = 0;
		else if (player_input == "LEFT")
			selected_corridor = 1;
		else if (player_input == "DOWN")
			selected_corridor = 2;
		else if (player_input == "RIGHT")
			selected_corridor = 3;

		// perform player movement
		can_move = this_board.move_player(player_to_move, selected_corridor);
		if (player_to_move.is_lit())
			this_board.illuminate(player_to_move);

		if (!can_move)
			std::cout << "YOU CAN'T MOVE DOWN THAT CORRIDOR. TRY AGAIN.\n";
	}
}

// fall_onto_tile()
// let a falling player determine where they want to land
void fall_onto_tile(player& falling_player, board& this_board)
{
	bool valid_spot = false;
	int spot_x = -1;
	int spot_y = -1;
	vector<tile_type> possbie_landing_tile_types = {straight_tile, t_tile, cross_tile, wax_eater, key_tile, gate_tile};
	shared_ptr<tile> new_landing_tile = this_board.new_random_tile(possbie_landing_tile_types);

	std::cout << "YOU'RE ABOUT TO HIT THE GROUND. ENTER THE POSITION OF WHERE YOU WANT TO LAND.\n";

	// get user input
	while (!valid_spot)
	{
		std::string player_input = get_valid_input([](std::string input) -> bool { 
			if (input.length() == 3  && std::isdigit(input[0]) && std::isdigit(input[2])) 
				return true; 

			return false; 
		});

		int spot_x = -1;
		spot_x = player_input[0] - '0';
		spot_y = player_input[2] - '0';

		// if the player entered a position that isn't on the row or column the specified
		if (
			falling_player.get_x() == -1 && spot_y != falling_player.get_y() ||
			falling_player.get_y() == -1 && spot_x != falling_player.get_x()
			)
		{
			continue;
		}

		shared_ptr<tile> landing_tile = this_board.play_area[spot_y][spot_x];
		if (landing_tile == nullptr)
			valid_spot = true;
	}

	this_board.place_tile(new_landing_tile, spot_x, spot_y);
	this_board.place_player(falling_player, spot_x, spot_y);

	if (falling_player.is_lit())
		this_board.illuminate(falling_player);
}

void start_game()
{
	board this_board = board();
	bool game_over = false;

	// init players
	for (int i = 0; i < 4; i++)
	{
		player new_player = player(i);
		this_board.players.push_back(new_player);
	}

	// start of game
	place_start_tiles(this_board);
	
	// main game loop
	while (!game_over)
	{
		for (player& current_player : this_board.players)
		{
			bool player_is_falling = current_player.is_falling();

			display_player_turn(current_player);
			move_player(current_player, this_board);
			this_board.darkness();
			this_board.display();
		}
	}

	this_board.~board();
}