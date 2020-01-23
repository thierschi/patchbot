#pragma once
#pragma once
#include "pb_map.h"
#include "pb_img.h"

#include <string>
#include <fstream>
#include <memory>

class pb_input {
public:
	static tile_map read_map_txt(const std::string& path);
	static tga read_tga_img(const std::string& path);
};

class pb_output {
public:
	static void write_map_txt(const std::string& path, const tile_map& t_map);
	static void print_map(const tile_map& t_map);
	static void print_map(const robot_map& r_map);
	static void write_tga_img(const std::string& path, tga& img);
};