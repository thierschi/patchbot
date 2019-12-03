#pragma once
#include <vector>

/*
	Enumerations fpr Terrain-types, Robot-types and Action-types
*/
enum class terrain {
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

enum class robot {
	PATCHBOT = 'p',
	BUGGER = '1',
	PUSHER = '2',
	DIGGER = '3',
	SWIMMER = '4',
	FOLLOWER = '5',
	HUNTER = '6',
	SNIFFER = '7'
};

enum class action {
	WALK,
	DIE,
	WIN,
	WALK_AND_WAIT,
	WAIT,
	OBSTRUCTED,
	DIG
};

/*
	The tile object is the object that is saved in the tile map so that the 
	tiles can have their own functionality
*/
class tile {
protected:
	terrain tile_terrain;

public:
	/*
		Constructor always checks, that passed Terrain-type is suitable for the 
		child class, throws an exception if it is not and sets the tile_terrain 
		after a successfull check
	*/
	tile(terrain t = terrain::STEEL_PLANKS);

	terrain get_terrain() const;

	/*
		Overloaded function intertact has the purpose of providing a uniform way
		to interact with tiles, so that this method can tell what happens when 
		a robot tries to walk on it / interact with it
	*/
	virtual action interact(robot r);
};

class startingpoint : public tile {
public:
	robot starting;
	startingpoint(terrain t);
};

class danger : public tile {
public:
	danger(terrain t);

	action interact(robot r) override;
};

class obstacle : public tile {
public:
	obstacle(terrain t);

	action interact(robot r) override;
};

class door : public tile {
protected:
	bool is_open = false;

public:
	door(terrain t);

	action interact(robot r) override;
};

class wall : public tile {
public:
	wall(terrain t);

	action interact(robot r) override;
};

class server : public tile {
public:
	server();

	action interact(robot r) override;
};

/*
	Tile_map is implemented as one dimensional vector  "i_map", where each 
	coordinate-pair x and y can be mapped to its tile with y * width + x.
	Coordinates are starting from zero.
*/
class tile_map {
protected:
	int height;
	int width;
	std::vector<tile> i_map;

public:
	tile_map(int w = 1, int h = 1);

	// Getter :

	int get_size() const;
	int get_height() const;
	int get_width() const;
	tile get_tile(int x, int y) const;

	//Setter :

	void set_height(int h);
	void set_width(int w);
	void set_tile(const tile& t, int x, int y);
	void set_tile(char c, int x, int y);
};
