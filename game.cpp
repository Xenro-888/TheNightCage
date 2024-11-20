#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <limits>
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
std::string get_valid_input(std::function<bool(std::string)> ruleset)
{
	std::string player_input;
	bool valid_input = false;

	while (!valid_input)
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear input buffer
		std::getline(std::cin, player_input);

		// check if input is valid
		if (ruleset(player_input))
			valid_input = true;

		if (!valid_input)
			std::cout << "INVALID INPUT. TRY AGAIN.\n";
	}
	
	return player_input;
}

// place_start_tiles()
// Has each player place their start tile in their specified position.
void place_start_tiles(board& this_board)
{
	for (player& current_player : this_board.players) 
	{
		bool valid_input = false;
		shared_ptr<tile> player_start_tile = std::make_unique<tile>(tile(start_tile));
		std::function<bool(std::string)> input_ruleset = [](std::string input) 
		{
			if (input.length() == 3 && std::isdigit(input[0]) && std::isdigit(input[2]))
				return true;

			return false;
		};

		std::cout << "WHERE WOULD YOU LIKE TO PLACE YOUR STARTING TILE?\n";
		std::cout << "\"X Y\" (STARTING FROM THE TOP-RIGHT CORNER.)\n";
		std::string player_input = get_valid_input(input_ruleset);
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
	std::string player_input;
	int selected_corridor = -1;
	bool can_move = false;

	std::cout << "WHICH CORRIDOR DO YOU WANT TO WALK DOWN?\n";
	std::cout << "(UP, DOWN, LEFT, RIGHT)\n";

	while (!can_move)
	{
		// get player input
		while (player_input != "UP" && player_input != "DOWN" && player_input != "LEFT" && player_input != "RIGHT")
		{
			std::cin.clear();
			std::getline(std::cin, player_input);
		}

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
		std::cout << "PLAYER POSITION: " << player_to_move.get_x() << ", " << player_to_move.get_y();
		if (player_to_move.is_lit())
			this_board.illuminate(player_to_move);
		std::cout << "PLAYER ILLUMINATION\n";

		// tell the player if they did something wrong
		if (!can_move)
			std::cout << "YOU CAN'T MOVE THERE.\n";

		player_input.clear();
	}
}

void start_game()
{
	board this_board = board();
	bool game_over = false;

	// init players
	for (int i = 0; i < 4; i++)
	{
		this_board.players.push_back(player(i));
	}

	// start of game
	place_start_tiles(this_board);

	// main game loop
	while (!game_over)
	{
		for (player& current_player : this_board.players)
		{
			display_player_turn(current_player);
			move_player(current_player, this_board);
			this_board.darkness();
			this_board.display();
		}
	}

	this_board.~board();
}