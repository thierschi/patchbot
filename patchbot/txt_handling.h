#pragma once
#include "pb_map.h"

/*
	Extracts tiles from .txt-file at given path and writes them into t_map;
*/
class pb_input {
public:
	static void read_map_txt(const char*& path, Tile_map& t_map);
};

class pb_output {
public:
	static void write_map_txt(const Tile_map& t_map, const char*& path);
};