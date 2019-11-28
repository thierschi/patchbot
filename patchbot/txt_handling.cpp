#include "txt_handling.h"
#include "pb_map.h"
#include "pb_exceptions.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <string>

class dyn_array {
public:
	char* array;

	dyn_array(int r = 1, int c = 1) {
		array = new char[r];
	}

	~dyn_array() {
		delete[] array;
	};
};

void pb_input::read_map_txt(const char*& path, Tile_map& t_map) {
	std::ifstream map_txt;
	map_txt.open(path, std::ios_base::in);

	if (!map_txt)
		// Checking if file exists
		throw std::invalid_argument("Invalid path: File or Path does not exist.");

	std::string map_size[2];
	getline(map_txt, map_size[0]);
	getline(map_txt, map_size[1]);

	for (int i = 0; i < 2; i++) {
		for (char c : map_size[i]) {
			if (c == 0)
				break;
			if (c < 48 || c > 57)
				// Checks that given map_sizes are numerical
				throw Map_format_exception("Map-format-exception: Map's width and height must be numerical.");
		}
	}

	// Gets int values for the map's size
	const int width = atoi(map_size[0].c_str());
	const int height = atoi(map_size[1].c_str());

	if (height <= 0 || width <= 0)
		// Checks that map's width and height is at least 1
		throw Map_format_exception("Map-format-exception: Map's width and height must be greater than zero.");

	// Sets the Tile_map's width and height
	t_map.set_width(width);
	t_map.set_height(height);
	
	dyn_array map_line = dyn_array(width + 1);
	bool has_start = false;
	bool has_end = false;

	for (int i = 0; i < height; i++) {
		if (map_txt.eof())
			// Checks for too few lines, by reading End_of_file(eof) bit
			throw Map_format_exception("Map-format-exception: Map has fewer lines than specified.");

		map_txt.getline(map_line.array, width + 1);
		
		if (map_txt.fail()) 
			// Checks for too long lines, by reading failbit
			throw Map_format_exception("Map-format-exception: A line in the map is longer than specified.");

		for (int j = 0; j < width; j++) {
			if (map_line.array[j] == 0)
				// Checks for too short lines, by checking for null terminator
				throw Map_format_exception("Map-format-exception: A line in the map is shorter than specified.");
			if (map_line.array[j] == 'P')
				has_end = true;
			if (map_line.array[j] == 'p') {
				if (has_start)
					// Checks if there was already a starting point for Patchbot
					throw Map_format_exception("Map-format-exception: The map must only have one startingpoint for PATCHBOT");
				has_start = true;
			}
			t_map.set_tile(map_line.array[j], j, i);
		}
	}

	if (!map_txt.eof())
		// Checks if there are unread lines over, by checking if the eof bit has not yet been set
		throw Map_format_exception("Map-format-exception: Map has more lines than specified.");

	if (!has_start || !has_end) 
		// Checks if start- or nedpoints are missing
		throw Map_format_exception("Map-format-exception: Missing starting point or / and endingpoint for PATCHBOT.");

	map_txt.close();
}

void pb_output::write_map_txt(const Tile_map& t_map, const char*& path) {
	std::ofstream map_txt;
	map_txt.open(path, std::ios_base::out);

	if (!map_txt)
		throw std::invalid_argument("Invalid path: File or Path does not exist.");

	map_txt << t_map.get_width() << std::endl << t_map.get_height();

	for (int i = 0; i < t_map.get_height(); i++) {
		map_txt << std::endl;
		for (int j = 0; j < t_map.get_width(); j++) {
			map_txt << (char) t_map.get_tile(j, i).get_terrain();
		}
	}
}