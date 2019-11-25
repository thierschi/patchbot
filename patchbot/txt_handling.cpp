#include "Tile_map.h"
#include "Pb_exceptions.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#define IS_ELEMENT_OF(arr, element) (std::find(std::begin(arr), std::end(arr), element) != std::end(arr))


using namespace std;


void read_map_txt(const char*& path, Tile_map &t_map) {
	/*
		Extracts tiles from .txt-file at given path and writes them into t_map;
	*/
	ifstream map_txt;
	map_txt.open(path, ios_base::in);

	if (!map_txt) 
		// Checking if file exists
		throw invalid_argument("Invalid path: File or Path does not exist.");

	char map_size[2][6];
	map_txt.getline(map_size[0], 6);
	map_txt.getline(map_size[1], 6);
	for (int i = 0; i < 2; i++) {
		if (!IS_ELEMENT_OF(map_size[i], 0))
			// Checking for too large map sizes, by looking for the null terminator. If not found the size is too big
			throw Map_format_exception("Map-format-exception: Map's width and height of the map must be smaller than 100.000.");
	}

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
	const int width = atoi(map_size[0]);
	const int height = atoi(map_size[1]);

	if (height <= 0 || width <= 0)
		// Checks that map's width and height is at least 1
		throw Map_format_exception("Map-format-exception: Map's width and height must be greater than zero.");

	// Sets the Tile_map's width and height
	t_map.set_width(width);
	t_map.set_height(height);
	
	char* map_line = new char[width + 1];
	bool has_start = false;
	bool has_end = false;

	for (int i = 0; i < height; i++) {
		if (map_txt.eof())
			// Checks for too few lines, by reading End_of_file(eof) bit
			throw Map_format_exception("Map-format-exception: Map has fewer lines than specified.");

		map_txt.getline(map_line, width + 1);
		
		if (map_txt.fail()) 
			// Checks for too long lines, by reading failbit
			throw Map_format_exception("Map-format-exception: A line in the map is longer than specified.");

		for (int j = 0; j < width; j++) {
			if (map_line[j] == 0)
				// Checks for too short lines, by checking for null terminator
				throw Map_format_exception("Map-format-exception: A line in the map is shorter than specified.");
			if (map_line[j] == 'P')
				has_end = true;
			if (map_line[j] == 'p') {
				if (has_start)
					// Checks if there was already a starting point for Patchbot
					throw Map_format_exception("Map-format-exception: The map must only have one startingpoint for PATCHBOT");
				has_start = true;
			}
			t_map.set_tile(map_line[j], j, i);
		}
	}

	if (!map_txt.eof())
		// Checks if there are unread lines over, by checking if the eof bit has not yet been set
		throw Map_format_exception("Map-format-exception: Map has more lines than specified.");

	if (!has_start || !has_end) 
		// Checks if start- or nedpoints are missing
		throw Map_format_exception("Map-format-exception: Missing starting point or / and endingpoint for PATCHBOT.");

	map_txt.close();
	delete[] map_line;
}

void write_map_txt(const Tile_map& t_map, const char*& path) {
	ofstream map_txt;
	map_txt.open(path, ios_base::out);

	if (!map_txt)
		throw invalid_argument("Invalid path: File or Path does not exist.");

	map_txt << t_map.get_width() << endl << t_map.get_height();

	for (int i = 0; i < t_map.get_height(); i++) {
		map_txt << endl;
		for (int j = 0; j < t_map.get_width(); j++) {
			map_txt << (char) t_map.get_tile(j, i).get_terrain();
		}
	}
}