#pragma once
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include "Commons.h"
#define IS_ELEMENT_OF(arr, element) (std::find(std::begin(arr), std::end(arr), element) != std::end(arr))

Terrain dangers[] = {
	Terrain::ABYSS,
	Terrain::WATER
};

Terrain obstacles[] = {
	Terrain::ALIEN_GRASS,
	Terrain::GRAVEL,
	Terrain::SECRET_PASSAGE
};

Terrain doors[] = {
	Terrain::MANUAL_DOOR,
	Terrain::AUTOMATIC_DOOR
};

Terrain walls[] = {
	Terrain::CONCRETE_WALL,
	Terrain::ROCK_WALL
};

Robot robots_with_wheels[] = {
	Robot::PATCHBOT,
	Robot::PUSHER,
	Robot::DIGGER,
	Robot::SWIMMER
};

class TileX {
protected:
	Terrain tile_terrain;

public:
	Tile(Terrain t = Terrain::STEEL_PLANKS) {
		if (t != Terrain::STEEL_PLANKS) {
			throw std::invalid_argument("Terrain missmatch. Use child class instead.");
		}
		tile_terrain = t;
	}

	Terrain get_terrain() {
		return tile_terrain;
	}

	Action interact(Robot r) {
		return Action::WALK;
	}
};

class Startingpoint : public Tile {
protected:
	Robot starting;

public:
	Startingpoint(Robot r) {
		starting = r;
		tile_terrain = (r == Robot::PATCHBOT) ? Terrain::PATCHBOT_START : Terrain::ENEMY_START;
	};
};

class Danger : public Tile {
public:
	Danger(Terrain t) {
		if (!IS_ELEMENT_OF(dangers, t)) {
			throw std::invalid_argument("Terrain missmatch.");
		}
		tile_terrain = t;
	}

	Action interact(Robot r) {
		if (r == Robot::SWIMMER && tile_terrain == Terrain::WATER) {
			return Action::WALK;
		}
		return Action::DIE;
	}
};

class Obstacle : public Tile {
public:
	Obstacle(Terrain t) {
		if (!IS_ELEMENT_OF(obstacles, t)) {
			throw std::invalid_argument("Terrain missmatch.");
		}
		tile_terrain = t;
	}

	Action interact(Robot r) {
		if (tile_terrain == Terrain::SECRET_PASSAGE) {
			return (r == Robot::PATCHBOT) ? Action::WALK : Action::OBSTRUCTED;
		}
		if (IS_ELEMENT_OF(robots_with_wheels, r)) {
			return (tile_terrain == Terrain::ALIEN_GRASS) ? Action::WALK_AND_WAIT : Action::WALK;
		}
		return (tile_terrain == Terrain::GRAVEL) ? Action::WALK_AND_WAIT : Action::WALK;
	}
};

class Door : public Tile {
protected:
	bool is_open = false;

public:
	Door(Terrain t) {
		if (!IS_ELEMENT_OF(doors, t)) {
			throw std::invalid_argument("Terrain missmatch.");
		}
		tile_terrain = t;
	}

	Action interact(Robot r) {
		// TODO: Door closing mechanismn
		if (is_open) {
			return Action::WALK;
		}
		if (tile_terrain == Terrain::AUTOMATIC_DOOR) {
			if (r == Robot::PATCHBOT) {
				return Action::OBSTRUCTED;
			}
			// open()
			return Action::WAIT;
		}
		// open()
		return Action::WAIT;
	}
};

class Wall : public Tile {
public:
	Wall(Terrain t) {
		if (!IS_ELEMENT_OF(walls, t)) {
			throw std::invalid_argument("Terrain missmatch.");
		}
		tile_terrain = t;
	}

	Action interact(Robot r) {
		if (r != Robot::DIGGER) {
			return Action::OBSTRUCTED;
		}
		return Action::DIG;
	}
};

class Server : public Tile {
public:
	Server() {
		tile_terrain = Terrain::MAIN_SERVER;
	}

	Action interact(Robot r) {
		if (r == Robot::PATCHBOT) {
			return Action::WIN;
		}
		return Action::OBSTRUCTED;
	}
};