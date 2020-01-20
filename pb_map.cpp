#pragma once
#include "pb_map.h"
#include "pb_exceptions.h"
#include "pb_img.h"

#include <iostream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <math.h>

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

const robot_type robots_with_wheels[] = {
	robot_type::PATCHBOT,
	robot_type::PUSHER,
	robot_type::DIGGER,
	robot_type::SWIMMER
};

robot::robot(robot_type _type, bool _is_dead) :
	type(_type),
	is_dead(_is_dead) 
{};

robot_map::robot_map(int _width, int _height) :
	width(_width),
	height(_height),
	has_pb(false)
{
	/*
		A new robot_map is always filled with robot_type NONE.
	*/
	robots.insert(robots.begin(), width * height, robot());
}

int robot_map::get_size() const {
	return width * height;
}

int robot_map::get_height() const {
	return height;
}

int robot_map::get_width() const {
	return width;
}

robot robot_map::get_robot(int x, int y) const {
	if (x >= width || y >= height)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Coordinates out of range.");
	return robots[y * width + x];
}

void robot_map::set_height(int _height) {
	/* Setting height by either appending
	width * new-height - old-height tiles
	or removing width * old-height - new-height tiles
	from the end */
	if (_height <= 0)
		throw std::invalid_argument("Invalid argument passed to robot_map: "
			"Height of robot_map must be 1 or greater.");
	else if (_height < height) {
		robots.erase(robots.begin() + _height * width, 
			robots.end());
		height = _height;
	}
	else if (_height > height) {
		robots.insert(robots.end(), (_height - height) * width, 
			robot());
		height = _height;
	}
}

void robot_map::set_width(int _width) {
	/*
		Setting width by either removing the last
		new-width - old-with tiles in every section
		of old-width tiles or adding
		new-width - old-width new tiles
		every old-with times.
		In both cases starting from the back of the map.
	*/
	if (_width <= 0)
		throw std::invalid_argument("Invalid argument passed to robot_map: "
			"Width of robot_map must be 1 or greater.");
	else if (_width < width) {
		for (int i = height; i > 0; i--)
			robots.erase(robots.begin() + ((i - 1) * width) + _width, 
				robots.begin() + i * width);
		width = _width;
	}
	else if (_width > width) {
		robots.push_back(robot());
		for (int i = height; i > 0; i--)
			robots.insert(robots.begin() + i * width, _width - width, 
				robot());
		robots.pop_back();
		width = _width;
	}
}

void robot_map::set_robot(const robot& _robot, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Coordinates out of range.");
	if (_robot.type == robot_type::PATCHBOT && has_pb) 
		throw std::invalid_argument(
			"Invalid argument passed to robot_map: "
			"This robot_map already has a patchbot.");
	robots[y * width + x] = _robot;
	has_pb = (_robot.type == robot_type::PATCHBOT) ? true : has_pb;
}

tile::tile(terrain t) :
	is_open(false),
	tile_terrain(t)
{
	if (t != terrain::STEEL_PLANKS)
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
}

terrain tile::get_terrain() const {
	return tile_terrain;
}

action tile::interact(robot_type r) {
	return action::WALK;
}

bool tile::get_is_open() const
{
	return is_open;
}


startingpoint::startingpoint(terrain t) :
	starting((robot_type)t)
{
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

danger::danger(terrain t)
{
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

action danger::interact(robot_type r) {
	if (r == robot_type::SWIMMER
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

action obstacle::interact(robot_type r) {
	if (tile_terrain == terrain::SECRET_PASSAGE)
		return (r == robot_type::PATCHBOT)
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

action door::interact(robot_type r) {
	/*
		TODO: Door opening and closing methods.
		Will be implemented in the future.
	*/
	if (is_open)
		return action::WALK;
	if (tile_terrain == terrain::AUTOMATIC_DOOR) {
		if (r == robot_type::PATCHBOT)
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

action wall::interact(robot_type r) {
	if (r != robot_type::DIGGER)
		return action::OBSTRUCTED;
	return action::DIG;
}

server::server() {
	tile_terrain = terrain::MAIN_SERVER;
}

action server::interact(robot_type r) {
	if (r == robot_type::PATCHBOT)
		return action::WIN;
	return action::OBSTRUCTED;
}

tile_map::tile_map(std::string _name, int _width, int _height) :
	name(_name),
	width(_width),
	height(_height),
	has_pb_start(false),
	robots(robot_map(_width, _height))
{
	/* A new Tile_map is always filled with the standard
	Tile object more accurate with STEEL_PLANKS */
	i_map.insert(i_map.begin(), _width * _height, tile());
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
	robots.set_height(h);
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
	robots.set_width(w);
}

void tile_map::set_tile(const tile& t, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	i_map[y * width + x] = t;
	
}

void tile_map::set_tile(const startingpoint& t, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	if (t.starting == robot_type::PATCHBOT && has_pb_start)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"This map already has a startingpoint for patchbot.");

	i_map[y * width + x] = t;
	robots.set_robot(robot(t.starting), x, y);
	has_pb_start = (t.starting == robot_type::PATCHBOT) ? true : has_pb_start;
}

void tile_map::set_tile(char c, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");

	switch (c) {
	case 'p':
		if (has_pb_start)
			throw std::invalid_argument(
				"Invalid argument passed to Tile_map: "
				"This map already has a startingpoint for patchbot.");
		i_map[y * width + x] = startingpoint(
			terrain::PATCHBOT_START);
		robots.set_robot(robot(robot_type::PATCHBOT), x, y);
		has_pb_start = true;
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
		robots.set_robot(robot(robot_type::BUGGER), x, y);
		break;
	case '2':
		i_map[y * width + x] = startingpoint(
			terrain::PUSHER_START);
		robots.set_robot(robot(robot_type::PUSHER), x, y);
		break;
	case '3':
		i_map[y * width + x] = startingpoint(
			terrain::DIGGER_START);
		robots.set_robot(robot(robot_type::DIGGER), x, y);
		break;
	case '4':
		i_map[y * width + x] = startingpoint(
			terrain::SWIMMER_START);
		robots.set_robot(robot(robot_type::SWIMMER), x, y);
		break;
	case '5':
		i_map[y * width + x] = startingpoint(
			terrain::FOLLOWER_START);
		robots.set_robot(robot(robot_type::FOLLOWER), x, y);
		break;
	case '6':
		i_map[y * width + x] = startingpoint(
			terrain::HUNTER_START);
		robots.set_robot(robot(robot_type::HUNTER), x, y);
		break;
	case '7':
		i_map[y * width + x] = startingpoint(
			terrain::SNIFFER_START);
		robots.set_robot(robot(robot_type::SNIFFER), x, y);
		break;
	default:
		throw map_format_exception(
			"Map-format-exception: "
			"Unknown character in map.");
	}
}