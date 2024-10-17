#include "player.h"

int player::get_x() { return x; }
int player::get_y() { return y; }
int player::get_color() { return color; }
bool player::is_lit() { return lit; }

bool player::is_falling()
{
	if (x == -1 || y == -1)
		return true;
	return false;
}

void player::set_x(int x) { this->x = x; }
void player::set_y(int y) { this->y = y; }
void player::set_lit(bool lit) { this->lit = lit; }

player::player(int color)
{
	nerve = 0;
	this->lit = true;
	this->color = color;
	x = -1;
	y = -1;
}
