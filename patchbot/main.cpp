#include <iostream>
#include "Commons.h"
#include "Tile.h"
#include "Tile_map.h"

using namespace std;

int main () {
	cout << "Hello World!\n";
	
	Tile tile1 = Tile();
	Tile tile2 = Tile(Terrain::STEEL_PLANKS);
	Danger danger1 = Danger(Terrain::ABYSS);

	Tile_map tm = Tile_map(5, 7);
	//tm.i_map.erase(tm.i_map.begin() + 4);
	//tm.printMap();
	
	cout << "Width: " << tm.get_width() << endl;
	cout << "Size: " << tm.get_size() << endl;
	//tm.setWidth(7);
	tm.set_width(10);
	tm.set_height(10);
	cout << "\n\n\n";
	tm.print_map();
	cout << "Width: " << tm.get_width() << endl;
	cout << "Size: " <<  tm.get_size() << endl;
	tm.set_tile(Door(Terrain::MANUAL_DOOR), 4, 6);
	tm.print_map();
	cout << (int) tm.get_tile(4, 6).get_terrain() << endl;

	/*cout << (int)tile1.get_terrain() << endl;
	cout << (int)danger1.get_terrain() << endl;
	cout << (int)tile1.interact(robot::patchbot) << endl;
	cout << (int)danger1.interact(robot::patchbot) << endl;*/

	return 0;
}