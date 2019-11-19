#include <iostream>
#include "Commons.h"
#include "Tile.h"
using namespace std;

int main () {
	cout << "Hello World!\n";
	
	Tile tile1 = Tile();
	Tile tile2 = Tile(Terrain::STEEL_PLANKS);
	Danger danger1 = Danger(Terrain::ABYSS);

	cout << (int)tile1.get_terrain() << endl;
	cout << (int)danger1.get_terrain() << endl;
	cout << (int)tile1.interact(Robot::PATCHBOT) << endl;
	cout << (int)danger1.interact(Robot::PATCHBOT) << endl;

	return 0;
}