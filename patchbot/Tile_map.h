#pragma once
#include "Tile.h"

#include <vector>

class Tile_map {
protected:
	int height;
	int width;
	std::vector<Tile> i_map;

public:
	Tile_map(int w = 1, int h = 1);

	void print_map();

	// Getter
	int get_size();
	int get_height();
	int get_width();
	Tile get_tile(int x, int y);

	//Setter
	void set_height(int h);
	void set_width(int w);
	void set_tile(Tile t, int x, int y);
};

