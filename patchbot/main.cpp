#include <iostream>
#include "Commons.h"
#include "Tile.h"
using namespace std;

int main () {
	cout << "Hello World!\n";

	Tile tile = Tile(terrain::DOOR, true);
	Tile tile1 = Tile(terrain::FLOOR);

	if (tile.get_tile_terrain() == terrain::DOOR) {
		cout << "There is a door here\n";
	}
	else {
		cout << "there's no door here\n";
	}

	cout << (int) tile.get_tile_terrain();
	cout << (int) tile1.get_tile_terrain();
	return 0;
}