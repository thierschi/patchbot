#pragma once
/* Enumerations fpr Terrain - types, Robot - types and Action - types */
enum class terrain {
	STEEL_PLANKS = ' ',
	PATCHBOT_START = 'p',
	ENEMY_START = 'e',
	ABYSS = 'O',
	WATER = '~',
	MAIN_SERVER = 'P',
	ALIEN_GRASS = 'g',
	GRAVEL = '.',
	SECRET_PASSAGE = 'x',
	MANUAL_DOOR = 'd',
	OPEN_MANUAL_DOOR = 'c',
	AUTOMATIC_DOOR = 'D',
	OPEN_AUTOMATIC_DOOR = 'C',
	CONCRETE_WALL = '#',
	ROCK_WALL = 'M'
};

enum class robot_type {
	PATCHBOT = 'p',
	BUGGER = '1',
	PUSHER = '2',
	DIGGER = '3',
	SWIMMER = '4',
	FOLLOWER = '5',
	HUNTER = '6',
	SNIFFER = '7',
	NONE = '0',
	DEAD = 'X'
};

enum class action {
	WALK,
	DIE,
	WIN,
	WALK_AND_WAIT,
	WAIT,
	OBSTRUCTED,
	DIG,
	OPEN_DOOR
};

enum class direction {
	NORTH,
	EAST,
	SOUTH,
	WEST,
	UNDEFINED,
	SOURCE
};

class coords {
public:
	int x, y;

	coords(int x = 0, int y = 0);

	friend inline bool operator==(const coords& lvalue, const coords& rvalue);
};

inline bool operator==(const coords& lvalue, const coords& rvalue)
{
	return lvalue.x == rvalue.x && lvalue.y == rvalue.y;
}

/*
	Class robot exists so that one can save a alive state (is_dad) for each
	robot.
*/
class robot {
public:
	robot_type type;
	unsigned int id;
	bool is_dead;

	robot(robot_type type_ = robot_type::NONE, bool is_dead_ = false);

private:
	static unsigned int id_counter;
};

/*
	The tile object is the object that is saved in the tile map so that the
	tiles can have their own functionality
*/
class tile {
protected:
	terrain tile_terrain;

public:
	/* Arrays of terrain types */
	static const terrain dangers[];
	static const terrain obstacles[];
	static const terrain doors[];
	static const terrain walls[];
	static const robot_type robots_with_wheels[];

	/* Properties important for dijkstra algorithmn */
	direction predecessor;

	/* Constructor always checks, that passed Terrain-type is suitable for the
	child class, throws an exception if it is not and sets the tile_terrain
	after a successfull check */
	tile(terrain t = terrain::STEEL_PLANKS);
	tile(const tile&) = default;
	virtual ~tile() = default;

	terrain get_terrain() const;

	/* Overloaded function intertact has the purpose of providing a uniform way
	to interact with tiles, so that this method can tell what happens when
	a robot tries to walk on it / interact with it */
	virtual action interact(robot_type r);

	/* Geht weight of edge going to tile */
	virtual int get_weight();
};

class startingpoint : public tile {
public:
	robot_type starting;
	startingpoint(robot_type t);
	startingpoint(const startingpoint&) = default;

	action interact(robot_type r) override;

	int get_weight() override;
};

class danger : public tile {
public:
	danger(terrain t);
	danger(const danger&) = default;

	action interact(robot_type r) override;

	int get_weight() override;
};

class obstacle : public tile {
public:
	obstacle(terrain t);
	obstacle(const obstacle&) = default;

	action interact(robot_type r) override;

	int get_weight() override;
};

class door : public tile {
public:
	door(terrain t);
	door(const door&) = default;

	action interact(robot_type r) override;

	int get_weight() override;
};

class wall : public tile {
public:
	wall(terrain t);
	wall(const wall&) = default;

	action interact(robot_type r) override;

	int get_weight() override;
};

class server : public tile {
public:
	server();
	server(const server&) = default;

	action interact(robot_type r) override;

	int get_weight() override;
};


