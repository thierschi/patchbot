#include "Tile.h"
#include "pb_enums.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#define IS_ELEMENT_OF(arr, element) (std::find(std::begin(arr), std::end(arr), element) != std::end(arr))

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

Tile::Tile(const Terrain &t) {
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

Action Tile::interact(const Robot &r) {
	return Action::WALK;
}

Startingpoint::Startingpoint(const Terrain &t) {
	if (t == Terrain::PATCHBOT_START) {
		tile_terrain = t;
		return;
	}
	if ((char)t < '1' || '7' < (char)t) // Check for the other robots (1-7)
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Danger::Danger(const Terrain &t) {
	if (!IS_ELEMENT_OF(dangers, t))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Action Danger::interact(const Robot &r) {
	if (r == Robot::SWIMMER && tile_terrain == Terrain::WATER) 
		return Action::WALK;
	return Action::DIE;
}

Obstacle::Obstacle(const Terrain &t) {
	if (!IS_ELEMENT_OF(obstacles, t))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Action Obstacle::interact(const Robot &r) {
	if (tile_terrain == Terrain::SECRET_PASSAGE)
		return (r == Robot::PATCHBOT) ? Action::WALK : Action::OBSTRUCTED;
	if (IS_ELEMENT_OF(robots_with_wheels, r))
		return (tile_terrain == Terrain::ALIEN_GRASS) ? Action::WALK_AND_WAIT : Action::WALK;
	return (tile_terrain == Terrain::GRAVEL) ? Action::WALK_AND_WAIT : Action::WALK;
}

Door::Door(const Terrain &t) {
	if (!IS_ELEMENT_OF(doors, t))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}
	
Action Door::interact(const Robot &r) {
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

Wall::Wall(const Terrain &t) {
	if (!IS_ELEMENT_OF(walls, t))
		throw std::invalid_argument("Invalid argument passed to constructor of tile: Terrain class missmatch.");
	tile_terrain = t;
}

Action Wall::interact(const Robot &r) {
	if (r != Robot::DIGGER)
		return Action::OBSTRUCTED;
	return Action::DIG;
}

Server::Server() {
	tile_terrain = Terrain::MAIN_SERVER;
}

Action Server::interact(const Robot &r) {
	if (r == Robot::PATCHBOT)
		return Action::WIN;
	return Action::OBSTRUCTED;
}