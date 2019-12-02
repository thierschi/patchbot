#include "pb_io.h"
#include "pb_map.h"
#include "pb_exceptions.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <string>
#include <memory>

tile_map pb_input::read_map_txt(const char*& path) {
	std::ifstream map_txt;
	map_txt.open(path, std::ios_base::in);
	if (!map_txt.is_open())
		throw std::invalid_argument(
			"Could not open file: "
			"Check if path is valid.");

	std::string map_size[2];
	getline(map_txt, map_size[0]);
	getline(map_txt, map_size[1]);

	for (int i = 0; i < 2; i++) {
		for (char c : map_size[i]) {
			if (c == 0)
				break;
			if (c < 48 || c > 57)
				/* Checks that given map_sizes 
				are numerical */
				throw map_format_exception(
					"Map-format-exception: "
					"Map's width and height "
					"must be numerical.");
		}
	}

	// Gets int values for the map's size
	const int width = atoi(map_size[0].c_str());
	const int height = atoi(map_size[1].c_str());

	if (height <= 0 || width <= 0)
		/* Checks that map's width 
		and height is at least 1 */
		throw map_format_exception(
			"Map-format-exception: "
			"Map's width and height must "
			"be greater than zero.");

	// Init map and set the Tile_map's width and height
	tile_map t_map = tile_map();
	t_map.set_width(width);
	t_map.set_height(height);
	
	//dyn_array map_line = dyn_array(width + 1);
	std::unique_ptr<char[]> map_line = 
		std::make_unique<char[]>(width + 1);
	bool has_start = false;
	bool has_end = false;

	for (int i = 0; i < height; i++) {
		if (map_txt.eof())
			/* Checks for too few lines, 
			by reading End_of_file(eof) bit */
			throw map_format_exception(
				"Map-format-exception: "
				"Map has fewer lines than specified.");

		map_txt.getline(map_line.get(), width + 1);
		
		if (map_txt.fail()) 
			/* Checks for too long lines, 
			by reading failbit */
			throw map_format_exception(
				"Map-format-exception: "
				"A line in the map is longer "
				"than specified.");

		for (int j = 0; j < width; j++) {
			if (map_line[j] == 0)
				/* Checks for too short lines, 
				by checking for null terminator */
				throw map_format_exception(
					"Map-format-exception: "
					"A line in the map is shorter "
					"than specified.");
			if (map_line[j] == 'P')
				has_end = true;
			if (map_line[j] == 'p') {
				if (has_start)
					/* Checks if there was already a 
					starting point for Patchbot */
					throw map_format_exception(
						"Map-format-exception: "
						"The map must only have one "
						"startingpoint for PATCHBOT");
				has_start = true;
			}
			t_map.set_tile(map_line[j], j, i);
		}
	}

	if (!map_txt.eof())
		/* Checks if there are unread lines over, 
		by checking if the eof bit has not yet been set */
		throw map_format_exception(
			"Map-format-exception: "
			"Map has more lines than specified.");

	if (!has_start || !has_end) 
		// Checks if start- or nedpoints are missing
		throw map_format_exception(
			"Map-format-exception: "
			"Missing starting point or / and "
			"endingpoint for PATCHBOT.");

	map_txt.close();
	return t_map;
}

tga pb_input::read_tga_img(const char*& path) {
	std::ifstream img_file;
	img_file.open(path, std::ios_base::in 
		| std::ios_base::binary);
	if (!img_file.is_open())
		throw std::invalid_argument(
			"Could not open file: "
			"Check if path is valid.");

	tga img = tga::load_file(img_file);

	img_file.close();
	return img;
}

void pb_output::write_map_txt(const tile_map& t_map, 
	const char*& path) {
	std::ofstream map_txt;
	map_txt.open(path, std::ios_base::out);
	if (!map_txt.is_open())
		throw std::invalid_argument(
			"Could not open file: "
			"Check if path is valid.");

	map_txt << t_map.get_width() 
		<< std::endl 
		<< t_map.get_height();

	for (int i = 0; i < t_map.get_height(); i++) {
		map_txt << std::endl;
		for (int j = 0; j < t_map.get_width(); j++) {
			map_txt << (char) t_map
				.get_tile(j, i)
				.get_terrain();
		}
	}
}
 
void pb_output::print_map(const tile_map& t_map) {
	int w = t_map.get_width();
	int h = t_map.get_height();

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			std::cout << (char)t_map
				.get_tile(j, i)
				.get_terrain();
		}
		std::cout << std::endl;
	}
}

void pb_output::write_tga_img(const char*& path, 
	tga& img) {
	std::ofstream img_file;
	img_file.open(path, std::ios_base::out 
		| std::ios_base::binary);
	if (!img_file.is_open())
		throw std::invalid_argument(
			"Could not open file: "
			"Check if path is valid.");

	img_file.write(img.get_raw_data().get(), 
		img.data_size + 44);
	img_file.close();
}