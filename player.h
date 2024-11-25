#ifndef PLAYER_H
#define PLAYER_H

class player
{
	int x;
	int y;
	int color;
	bool lit;
	int nerve;

public:

	int get_x();
	int get_y();
	int get_color();
	bool is_lit();
	bool is_falling();

	void set_x(int x);
	void set_y(int y);
	void set_lit(bool lit);

	bool operator<(const player) const;

	player(int color);
};

#endif /* PLAYER_H */
