#include "Tile_map.h"
#include "Tile.h"

#include <iostream>
#include <vector>

#define SIZE height*width

using namespace std;

Tile_map::Tile_map(int w, int h) {
	height = h;
	width = w;

	i_map.insert(i_map.begin(), SIZE, Tile());
}

void Tile_map::print_map() {
	for (int i = 0; i < SIZE; i++) {
		cout << (int)i_map[i].get_terrain();
		if ((i + 1) % width == 0)
			cout << "\n";
	}
}

// Getter
int Tile_map::get_size() {
	return SIZE;
}

int Tile_map::get_height() {
	return height;
}

int Tile_map::get_width() {
	return width;
}

Tile Tile_map::get_tile(int x, int y) {
	if (x >= width || y >= height) {
		throw invalid_argument("Coordinates out of range.");
	}
	return i_map[y * width + x];
}

//Setter
void Tile_map::set_height(int h) {
	if (h <= 0) {
		throw invalid_argument("New height of Tile_map must be 1 or greater.");
	}
	else if (h < height) {
		i_map.erase(i_map.begin() + h * width, i_map.end());
		height = h;
	}
	else if (h > height) {
		i_map.insert(i_map.end(), (h - height) * width, Tile());
		height = h;
	}
}

void Tile_map::set_width(int w) {
	if (w <= 0) {
		throw invalid_argument("New width of Tile_map must be 1 or greater");
	}
	else if (w < width) {
		for (int i = height; i > 0; i--) {
			i_map.erase(i_map.begin() + ((i - 1) * width) + w, i_map.begin() + i * width);
		}
		width = w;
	}
	else if (w > width) {
		i_map.push_back(Tile());
		for (int i = height; i > 0; i--) {
			i_map.insert(i_map.begin() + i * width, w - width, Tile());
		}
		i_map.pop_back();
		width = w;
	}
}

void Tile_map::set_tile(Tile t, int x, int y) {
	if (x >= width || y >= height) {
		throw invalid_argument("Coordinates invalid.");
	}
	i_map[y * width + x] = t;
}