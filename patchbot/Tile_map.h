#pragma once
#include "Tile.h"

#include <vector>

/*
	Tile_map is implemented as one dimensional vector "i_map", where each coordinate-pair x and y can be mapped to its tile with y * width + x.
	Coordinates are starting from zero.
*/

class Tile_map {
protected:
	int height;
	int width;
	std::vector<Tile> i_map;

public:
	Tile_map(int w = 1, int h = 1);

	void print_map(); // Prints Tile_map to console for debugging

	// Getter
	int get_size() const;
	int get_height() const;
	int get_width() const;
	Tile get_tile(int x, int y) const;

	//Setter
	void set_height(int h);
	void set_width(int w);
	void set_tile(const Tile &t, int x, int y);
	void set_tile(char c, int x, int y);
};

