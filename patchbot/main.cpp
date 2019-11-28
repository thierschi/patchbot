#include "pb_map.h"
#include "txt_handling.h"

#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " [Path to input-.txt-file] [path to output-.txt-file]" << std::endl;
		return 0;
	}
	const char* i_path = argv[1];
	const char* o_path = argv[2];

	/*const char* i_path = "..\\..\\everything1.txt";
	const char* o_path = "..\\..\\everything2.txt";*/

	Tile_map map = Tile_map();

	try
	{
		std::cout << "Reading Tile-map from input-file at " << i_path << " ..." << std::endl;
		pb_input::read_map_txt(i_path, map);
		std::cout << "Read input-file successfully." << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << std::endl << "Exception " << e.what() << std::endl;
		return 0;
	}

	std::cout << std::endl << "Writing Tile-map to ouput-file at " << o_path << " ..." << std::endl;
	pb_output::write_map_txt(map, o_path);
	std::cout << "Wrote ouput-file successfully." << std::endl << std::endl;

	return 0;
}