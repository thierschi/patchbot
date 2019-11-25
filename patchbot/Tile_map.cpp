#include "Tile_map.h"
#include "Tile.h"
#include "Pb_exceptions.h"

#include <iostream>
#include <vector>

#define SIZE height*width

using namespace std;

Tile_map::Tile_map(int w, int h) {
	height = h;
	width = w;

	i_map.insert(i_map.begin(), SIZE, Tile()); // A new Tile_map is always filled with the standard Tile object more accurate with STEEL_PLANKS
}

void Tile_map::print_map() {
	for (int i = 0; i < SIZE; i++) {
		cout << (char) i_map[i].get_terrain() << " ";
		if ((i + 1) % width == 0)
			cout << "\n";
	}
}

// Getter
int Tile_map::get_size() const {
	return SIZE;
}

int Tile_map::get_height() const {
	return height;
}

int Tile_map::get_width()  const {
	return width;
}

Tile Tile_map::get_tile(int x, int y) const {
	if (x >= width || y >= height) 
		throw invalid_argument("Invalid argument passed to Tile_map: Coordinates out of range.");
	return i_map[y * width + x];
}

//Setter
void Tile_map::set_height(int h) {
	/*
		Setting height by either appending width * new-height - old-height tiles or removing width * old-height - new-height tiles from the end
	*/
	if (h <= 0) {
		throw invalid_argument("Invalid argument passed to Tile_map: Height of tile_map must be 1 or greater.");
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
	/*
		Setting width by either removing the last new-width - old-with tiles in every section of old-width tiles or adding new-width - old-width new tiles every old-with times.
		In both cases starting from the back of the map.
	*/
	if (w <= 0) {
		throw invalid_argument("Invalid argument passed to Tile_map: Width of tile_map must be 1 or greater.");
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

void Tile_map::set_tile(const Tile &t, int x, int y) { 
	if (x >= width || y >= height) 
		throw invalid_argument("Ínvalid argument passed to Tile_map: Coordinates out of range.");
	i_map[y * width + x] = t;
}

void Tile_map::set_tile(char c, int x, int y) {
	if (x >= width || y >= height) 
		throw invalid_argument("Ínvalid argument passed to Tile_map: Coordinates out of range.");

	switch (c) {
	case 'p':
		i_map[y * width + x] = Startingpoint(Terrain::PATCHBOT_START);
		break;
	case 'P':
		i_map[y * width + x] = Server();
		break;
	case ' ':
		i_map[y * width + x] = Tile();
		break;
	case '#':
		i_map[y * width + x] = Wall(Terrain::CONCRETE_WALL);
		break;
	case 'M':
		i_map[y * width + x] = Wall(Terrain::ROCK_WALL);
		break;
	case 'd':
		i_map[y * width + x] = Door(Terrain::MANUAL_DOOR);
		break;
	case 'D':
		i_map[y * width + x] = Door(Terrain::AUTOMATIC_DOOR);
		break;
	case 'g':
		i_map[y * width + x] = Obstacle(Terrain::ALIEN_GRASS);
		break;
	case '.':
		i_map[y * width + x] = Obstacle(Terrain::GRAVEL);
		break;
	case 'x':
		i_map[y * width + x] = Obstacle(Terrain::SECRET_PASSAGE);
		break;
	case 'O':
		i_map[y * width + x] = Danger(Terrain::ABYSS);
		break;
	case '~':
		i_map[y * width + x] = Danger(Terrain::WATER);
		break;
	case '1':
		i_map[y * width + x] = Startingpoint(Terrain::BUGGER_START);
		break;
	case '2':
		i_map[y * width + x] = Startingpoint(Terrain::PUSHER_START);
		break;
	case '3':
		i_map[y * width + x] = Startingpoint(Terrain::DIGGER_START);
		break;
	case '4':
		i_map[y * width + x] = Startingpoint(Terrain::SWIMMER_START);
		break;
	case '5':
		i_map[y * width + x] = Startingpoint(Terrain::FOLLOWER_START);
		break;
	case '6':
		i_map[y * width + x] = Startingpoint(Terrain::HUNTER_START);
		break;
	case '7':
		i_map[y * width + x] = Startingpoint(Terrain::SNIFFER_START);
		break;
	default:
		throw Map_format_exception("Map-format-exception: Unknown character in map.");
	}
}