#include "player.h"

int player::get_x() { return x; }
int player::get_y() { return y; }
int player::get_color() { return color; }
bool player::is_lit() { return lit; }

bool player::is_falling(){ return falling; }

void player::set_x(int x) { this->x = x; }
void player::set_y(int y) { this->y = y; }
void player::set_lit(bool lit) { this->lit = lit; }
void player::set_fall_state(bool new_fall_state) { falling = new_fall_state; };

bool player::operator<(const player player2) const
{
	if (player2.color < this->color)
		return true;
		
	return false;
}

player::player(int color)
{
	nerve = 2;
	this->lit = true;
	this->color = color;
	x = -1;
	y = -1;
}
