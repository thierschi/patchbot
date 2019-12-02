#pragma once
#include "pb_map.h"
#include "pb_img.h"

class pb_input {
public:
	static tile_map read_map_txt(const char*& path);
	static tga read_tga_img(const char*& path);
};

class pb_output {
public:
	static void write_map_txt(const tile_map& t_map, 
		const char*& path);
	static void print_map(const tile_map& t_map);
	static void write_tga_img(const char*& path, 
		tga& img);
};