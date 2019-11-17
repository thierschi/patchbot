#pragma once
#include <stdexcept>
#include "Commons.h"

class Tile
{
private:
	terrain tile_terrain;	

public:
	terrain get_tile_terrain() {
		return tile_terrain;
	}
	bool is_door_closed;

	Tile(terrain t_r) {
		if (t_r == terrain::DOOR) {
			throw std::invalid_argument("Tile terrain is DOOR: Missing closed state.");
		}
		tile_terrain = t_r;
	}

	Tile(terrain t_r, bool d_c) {
		if (t_r != terrain::DOOR) {
			throw std::invalid_argument("Tile terrain must be DOOR.");
		}
		tile_terrain = t_r;
		is_door_closed = d_c;
	}

};

