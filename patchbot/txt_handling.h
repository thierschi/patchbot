#pragma once
#include "Tile_map.h"

void read_map_txt(const char*& path, Tile_map &t_map);
void write_map_txt(const Tile_map& t_map, const char*& path);