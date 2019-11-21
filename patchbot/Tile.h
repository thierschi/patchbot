#pragma once
#include "Commons.h"

class Tile {
protected:
	Terrain tile_terrain;

public:
	Tile(Terrain t = Terrain::STEEL_PLANKS);

	Terrain get_terrain();

	Action interact(Robot r);
};

class Startingpoint : public Tile {
protected:
	Robot starting;

public:
	Startingpoint(Robot r);
};

class Danger : public Tile {
public:
	Danger(Terrain t);

	Action interact(Robot r);
};

class Obstacle : public Tile {
public:
	Obstacle(Terrain t);

	Action interact(Robot r);
};

class Door : public Tile {
protected:
	bool is_open = false;

public:
	Door(Terrain t);

	Action interact(Robot r);
};

class Wall : public Tile {
public:
	Wall(Terrain t);

	Action interact(Robot r);
};

class Server : public Tile {
public:
	Server();

	Action interact(Robot r);
};