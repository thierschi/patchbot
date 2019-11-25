#include <iostream>
#include "Tile_map.h"
#include "txt_handling.h"

using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " [Path to input-.txt-file] [path to output-.txt-file]" << endl;
		return 0;
	}
	const char* i_path = argv[1];
	const char* o_path = argv[2];

	/*const char* i_path = "..\\..\\everything1.txt";
	const char* o_path = "..\\..\\everything2.txt";*/

	Tile_map map = Tile_map();

	try
	{
		cout << "Reading Tile-map from input-file at " << i_path << " ..." << endl;
		read_map_txt(i_path, map);
		cout << "Read input-file successfully." << endl;
	}
	catch (const exception& e)
	{
		cout << endl << "Exception " << e.what() << endl;
		return 0;
	}

	cout << endl << "Writing Tile-map to ouput-file at " << o_path << " ..." << endl;
	write_map_txt(map, o_path);
	cout << "Wrote ouput-file successfully." << endl << endl;

	return 0;
}