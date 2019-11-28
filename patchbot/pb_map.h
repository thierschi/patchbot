#pragma once
#include <vector>

/*
	Enumerations fpr Terrain-types, Robot-types and Action-types
*/

enum class Terrain {
	STEEL_PLANKS = ' ',
	PATCHBOT_START = 'p',
	BUGGER_START = '1',
	PUSHER_START = '2',
	DIGGER_START = '3',
	SWIMMER_START = '4',
	FOLLOWER_START = '5',
	HUNTER_START = '6',
	SNIFFER_START = '7',
	ABYSS = 'O',
	WATER = '~',
	MAIN_SERVER = 'P',
	ALIEN_GRASS = 'g',
	GRAVEL = '.',
	SECRET_PASSAGE = 'x',
	MANUAL_DOOR = 'd',
	AUTOMATIC_DOOR = 'D',
	CONCRETE_WALL = '#',
	ROCK_WALL = 'M'
};

enum class Robot {
	PATCHBOT = 'p',
	BUGGER = '1',
	PUSHER = '2',
	DIGGER = '3',
	SWIMMER = '4',
	FOLLOWER = '5',
	HUNTER = '6',
	SNIFFER = '7'
};

enum class Action {
	WALK,
	DIE,
	WIN,
	WALK_AND_WAIT,
	WAIT,
	OBSTRUCTED,
	DIG
};

/*
	The tile object is the object that is saved in the tile map so that the tiles can have their own functionality
*/

class Tile {
protected:
	Terrain tile_terrain;

public:
	/*
		Constructor always checks, that passed Terrain-type is suitable for the child class, throws an exception if it is not and sets the tile_terrain after a successfull check
	*/
	Tile(Terrain t = Terrain::STEEL_PLANKS);

	Terrain get_terrain() const;

	/*
		Overloaded function intertact has the purpose of providing a uniform way to interact with tiles, so that this method can tell what happens when a robot tries to walk on it / interact with it
	*/
	virtual Action interact(Robot r);
};

class Startingpoint : public Tile {
public:
	Startingpoint(Terrain t);
};

class Danger : public Tile {
public:
	Danger(Terrain t);

	Action interact(Robot r) override;
};

class Obstacle : public Tile {
public:
	Obstacle(Terrain t);

	Action interact(Robot r) override;
};

class Door : public Tile {
protected:
	bool is_open = false;

public:
	Door(Terrain t);

	Action interact(Robot r) override;
};

class Wall : public Tile {
public:
	Wall(Terrain t);

	Action interact(Robot r) override;
};

class Server : public Tile {
public:
	Server();

	Action interact(Robot r) override;
};

/*
	Tile_map is implemented as one dimensional vector "i_map", where each coordinate-pair x and y can be mapped to its tile with y * width + x.
	Coordinates are starting from zero.
*/
class Tile_map {
protected:
	int height;
	int width;
	std::vector<Tile> i_map;

public:
	Tile_map(int w = 1, int h = 1);

	// Prints Tile_map to console for debugging
	void print_map() const;

	// Getter :

	int get_size() const;
	int get_height() const;
	int get_width() const;
	Tile get_tile(int x, int y) const;

	//Setter :

	void set_height(int h);
	void set_width(int w);
	void set_tile(const Tile& t, int x, int y);
	void set_tile(char c, int x, int y);
};
