#pragma once
#include "pb_enums.h"

/*
	The tile object is the object that is saved in the tile map so that the tiles can have their own functionality
*/

class Tile {
protected:
	Terrain tile_terrain;

public:
	Tile(const Terrain &t = Terrain::STEEL_PLANKS);

	Terrain get_terrain() const;

	/* 
		Overloaded function intertact has the purpose of providing a uniform way to interact with tiles, so that this method can tell what happens when a robot tries to walk on it / interact with it
	*/
	virtual Action interact(const Robot &r);
};

class Startingpoint : public Tile {
public:
	Startingpoint(const Terrain &t);
};

class Danger : public Tile {
public:
	Danger(const Terrain &t);

	Action interact(const Robot &r) override;
};

class Obstacle : public Tile {
public:
	Obstacle(const Terrain &t);

	Action interact(const Robot &r) override;
};

class Door : public Tile {
protected:
	bool is_open = false;

public:
	Door(const Terrain &t);

	Action interact(const Robot &r) override;
};

class Wall : public Tile {
public:
	Wall(const Terrain &t);

	Action interact(const Robot &r) override;
};

class Server : public Tile {
public:
	Server();

	Action interact(const Robot &r) override;
};