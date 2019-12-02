#pragma once
#include "pb_map.h"
#include "pb_exceptions.h"

#include <iostream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>

const terrain dangers[] = {
	terrain::ABYSS,
	terrain::WATER
};

const terrain obstacles[] = {
	terrain::ALIEN_GRASS,
	terrain::GRAVEL,
	terrain::SECRET_PASSAGE
};

const terrain doors[] = {
	terrain::MANUAL_DOOR,
	terrain::AUTOMATIC_DOOR
};

const terrain walls[] = {
	terrain::CONCRETE_WALL,
	terrain::ROCK_WALL
};

const robot robots_with_wheels[] = {
	robot::PATCHBOT,
	robot::PUSHER,
	robot::DIGGER,
	robot::SWIMMER
};

tile::tile(terrain t) {
	if (t != terrain::STEEL_PLANKS)
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

terrain tile::get_terrain() const {
	return tile_terrain;
}

action tile::interact(robot r) {
	return action::WALK;
}

startingpoint::startingpoint(terrain t) {
	if (t == terrain::PATCHBOT_START) {
		tile_terrain = t;
		return;
	}
	if ((char)t < '1' || '7' < (char)t) 
		// Check for the other robots (1-7)
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

danger::danger(terrain t) {
	if (!(
		(std::find(std::begin(dangers),
			std::end(dangers), 
			t) 
			!= std::end(dangers))))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action danger::interact(robot r) {
	if (r == robot::SWIMMER 
		&& tile_terrain 
		== terrain::WATER)
		return action::WALK;
	return action::DIE;
}

obstacle::obstacle(terrain t) {
	if (!((std::find(std::begin(obstacles), 
		std::end(obstacles), 
		t) 
		!= std::end(obstacles))))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action obstacle::interact(robot r) {
	if (tile_terrain == terrain::SECRET_PASSAGE)
		return (r == robot::PATCHBOT) 
			? action::WALK : action::OBSTRUCTED;
	if ((std::find(std::begin(robots_with_wheels),
		std::end(robots_with_wheels), 
		r) 
		!= std::end(robots_with_wheels)))
		return (tile_terrain == terrain::ALIEN_GRASS) 
		? action::WALK_AND_WAIT : action::WALK;
	return (tile_terrain == terrain::GRAVEL) 
		? action::WALK_AND_WAIT : action::WALK;
}

door::door(terrain t) {
	if (!(std::find(std::begin(doors),
		std::end(doors),
		t) 
		!= std::end(doors)))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action door::interact(robot r) {
	/*
		TODO: Door opening and closing methods. 
		Will be implemented in the future.
	*/
	if (is_open)
		return action::WALK;
	if (tile_terrain == terrain::AUTOMATIC_DOOR) {
		if (r == robot::PATCHBOT)
			return action::OBSTRUCTED;
		// open()
		return action::WAIT;
	}
	// open()
	return action::WAIT;
}

wall::wall(terrain t) {
	if (!((std::find(std::begin(walls), 
		std::end(walls), 
		t) 
		!= 
		std::end(walls))))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action wall::interact(robot r) {
	if (r != robot::DIGGER)
		return action::OBSTRUCTED;
	return action::DIG;
}

server::server() {
	tile_terrain = terrain::MAIN_SERVER;
}

action server::interact(robot r) {
	if (r == robot::PATCHBOT)
		return action::WIN;
	return action::OBSTRUCTED;
}

tile_map::tile_map(int w, int h) {
	width = w;
	height = h;

	/* A new Tile_map is always filled with the standard 
	Tile object more accurate with STEEL_PLANKS */
	i_map.insert(i_map.begin(), get_size(), tile());
}

// Getter
int tile_map::get_size() const {
	return width * height;
}

int tile_map::get_height() const {
	return height;
}

int tile_map::get_width()  const {
	return width;
}

tile tile_map::get_tile(int x, int y) const {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	return i_map[y * width + x];
}

//Setter
void tile_map::set_height(int h) {
	/* Setting height by either appending 
	width * new-height - old-height tiles
	or removing width * old-height - new-height tiles 
	from the end */
	if (h <= 0) {
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Height of tile_map must be 1 or greater.");
	}
	else if (h < height) {
		i_map.erase(i_map.begin() + h * width, 
			i_map.end());
		height = h;
	}
	else if (h > height) {
		i_map.insert(i_map.end(), 
			(h - height) * width, tile());
		height = h;
	}
}

void tile_map::set_width(int w) {
	/*
		Setting width by either removing the last 
		new-width - old-with tiles in every section 
		of old-width tiles or adding 
		new-width - old-width new tiles 
		every old-with times.
		In both cases starting from the back of the map.
	*/
	if (w <= 0) {
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Width of tile_map must be 1 or greater.");
	}
	else if (w < width) {
		for (int i = height; i > 0; i--) {
			i_map.erase(i_map.begin() 
				+ ((i - 1) * width) + w,
				i_map.begin() + i * width);
		}
		width = w;
	}
	else if (w > width) {
		i_map.push_back(tile());
		for (int i = height; i > 0; i--) {
			i_map.insert(i_map.begin() + i * width,
				w - width, tile());
		}
		i_map.pop_back();
		width = w;
	}
}

void tile_map::set_tile(const tile& t, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	i_map[y * width + x] = t;
}

void tile_map::set_tile(char c, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");

	switch (c) {
	case 'p':
		i_map[y * width + x] = startingpoint(
			terrain::PATCHBOT_START);
		break;
	case 'P':
		i_map[y * width + x] = server();
		break;
	case ' ':
		i_map[y * width + x] = tile();
		break;
	case '#':
		i_map[y * width + x] = wall(
			terrain::CONCRETE_WALL);
		break;
	case 'M':
		i_map[y * width + x] = wall(
			terrain::ROCK_WALL);
		break;
	case 'd':
		i_map[y * width + x] = door(
			terrain::MANUAL_DOOR);
		break;
	case 'D':
		i_map[y * width + x] = door(
			terrain::AUTOMATIC_DOOR);
		break;
	case 'g':
		i_map[y * width + x] = obstacle(
			terrain::ALIEN_GRASS);
		break;
	case '.':
		i_map[y * width + x] = obstacle(
			terrain::GRAVEL);
		break;
	case 'x':
		i_map[y * width + x] = obstacle(
			terrain::SECRET_PASSAGE);
		break;
	case 'O':
		i_map[y * width + x] = danger(
			terrain::ABYSS);
		break;
	case '~':
		i_map[y * width + x] = danger(
			terrain::WATER);
		break;
	case '1':
		i_map[y * width + x] = startingpoint(
			terrain::BUGGER_START);
		break;
	case '2':
		i_map[y * width + x] = startingpoint(
			terrain::PUSHER_START);
		break;
	case '3':
		i_map[y * width + x] = startingpoint(
			terrain::DIGGER_START);
		break;
	case '4':
		i_map[y * width + x] = startingpoint(
			terrain::SWIMMER_START);
		break;
	case '5':
		i_map[y * width + x] = startingpoint(
			terrain::FOLLOWER_START);
		break;
	case '6':
		i_map[y * width + x] = startingpoint(
			terrain::HUNTER_START);
		break;
	case '7':
		i_map[y * width + x] = startingpoint(
			terrain::SNIFFER_START);
		break;
	default:
		throw map_format_exception(
			"Map-format-exception: "
			"Unknown character in map.");
	}
}