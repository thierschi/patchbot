#pragma once
#include "pb_map.h"
#include "pb_exceptions.h"

#include <iostream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>

const Terrain dangers[] = {
	Terrain::ABYSS,
	Terrain::WATER
};

const Terrain obstacles[] = {
	Terrain::ALIEN_GRASS,
	Terrain::GRAVEL,
	Terrain::SECRET_PASSAGE
};

const Terrain doors[] = {
	Terrain::MANUAL_DOOR,
	Terrain::AUTOMATIC_DOOR
};

const Terrain walls[] = {
	Terrain::CONCRETE_WALL,
	Terrain::ROCK_WALL
};

const Robot robots_with_wheels[] = {
	Robot::PATCHBOT,
	Robot::PUSHER,
	Robot::DIGGER,
	Robot::SWIMMER
};

Tile::Tile(Terrain t) {
	/*
		Constructor always checks, that passed Terrain-type is suitable for the child class, throws an exception if it is not and sets the tile_terrain after a successfull check
	*/
	if (t != Terrain::STEEL_PLANKS)
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Terrain Tile::get_terrain() const {
	return tile_terrain;
}

Action Tile::interact(Robot r) {
	return Action::WALK;
}

Startingpoint::Startingpoint(Terrain t) {
	if (t == Terrain::PATCHBOT_START) {
		tile_terrain = t;
		return;
	}
	if ((char)t < '1' || '7' < (char)t) // Check for the other robots (1-7)
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Danger::Danger(Terrain t) {
	if (!((std::find(std::begin(dangers), std::end(dangers), t) != std::end(dangers))))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Action Danger::interact(Robot r) {
	if (r == Robot::SWIMMER && tile_terrain == Terrain::WATER)
		return Action::WALK;
	return Action::DIE;
}

Obstacle::Obstacle(Terrain t) {
	if (!((std::find(std::begin(obstacles), std::end(obstacles), t) != std::end(obstacles))))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Action Obstacle::interact(Robot r) {
	if (tile_terrain == Terrain::SECRET_PASSAGE)
		return (r == Robot::PATCHBOT) ? Action::WALK : Action::OBSTRUCTED;
	if ((std::find(std::begin(robots_with_wheels), std::end(robots_with_wheels), r) != std::end(robots_with_wheels)))
		return (tile_terrain == Terrain::ALIEN_GRASS) ? Action::WALK_AND_WAIT : Action::WALK;
	return (tile_terrain == Terrain::GRAVEL) ? Action::WALK_AND_WAIT : Action::WALK;
}

Door::Door(Terrain t) {
	if (!(std::find(std::begin(doors), std::end(doors), t) != std::end(doors)))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Action Door::interact(Robot r) {
	/*
		TODO: Door opening and closing methods. Will be implemented in the future.
	*/
	if (is_open)
		return Action::WALK;
	if (tile_terrain == Terrain::AUTOMATIC_DOOR) {
		if (r == Robot::PATCHBOT)
			return Action::OBSTRUCTED;
		// open()
		return Action::WAIT;
	}
	// open()
	return Action::WAIT;
}

Wall::Wall(Terrain t) {
	if (!((std::find(std::begin(walls), std::end(walls), t) != std::end(walls))))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Action Wall::interact(Robot r) {
	if (r != Robot::DIGGER)
		return Action::OBSTRUCTED;
	return Action::DIG;
}

Server::Server() {
	tile_terrain = Terrain::MAIN_SERVER;
}

Action Server::interact(Robot r) {
	if (r == Robot::PATCHBOT)
		return Action::WIN;
	return Action::OBSTRUCTED;
}

Tile_map::Tile_map(int w, int h) {
	width = w;
	height = h;

	i_map.insert(i_map.begin(), get_size(), Tile()); // A new Tile_map is always filled with the standard Tile object more accurate with STEEL_PLANKS
}

void Tile_map::print_map() const {
	for (int i = 0; i < get_size(); i++) {
		std::cout << (char)i_map[i].get_terrain() << " ";
		if ((i + 1) % width == 0)
			std::cout << "\n";
	}
}

// Getter
int Tile_map::get_size() const {
	return width * height;
}

int Tile_map::get_height() const {
	return height;
}

int Tile_map::get_width()  const {
	return width;
}

Tile Tile_map::get_tile(int x, int y) const {
	if (x >= width || y >= height)
		throw std::invalid_argument("Invalid argument passed to Tile_map: Coordinates out of range.");
	return i_map[y * width + x];
}

//Setter
void Tile_map::set_height(int h) {
	/*
		Setting height by either appending width * new-height - old-height tiles or removing width * old-height - new-height tiles from the end
	*/
	if (h <= 0) {
		throw std::invalid_argument("Invalid argument passed to Tile_map: Height of tile_map must be 1 or greater.");
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
		throw std::invalid_argument("Invalid argument passed to Tile_map: Width of tile_map must be 1 or greater.");
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

void Tile_map::set_tile(const Tile& t, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument("Ínvalid argument passed to Tile_map: Coordinates out of range.");
	i_map[y * width + x] = t;
}

void Tile_map::set_tile(char c, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument("Ínvalid argument passed to Tile_map: Coordinates out of range.");

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