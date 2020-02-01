#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <queue>

/*
	Enumerations fpr Terrain-types, Robot-types and Action-types
*/
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

typedef std::pair<int, int> int_pair;

class coords {
public:
	int x, y;
	
	coords(int x = 0, int y = 0);
};

/*
	Class robot exists so that one can save a alive state (is_dad) for each 
	robot.
*/
class robot {
public:
	robot_type type;
	bool is_dead;

	robot(robot_type type_ = robot_type::NONE, bool is_dead_ = false);
};

/*
	Robot_map is implemented as one dimensional vector  "__robot_map", where 
	each coordinate-pair x and y can be mapped to its tile with y * width + x.
	Coordinates are starting from zero.
*/
class robot_map {
protected:
	int height;
	int width;
	bool has_pb;
	std::vector<robot> robots;
	std::unordered_map<int, bool> graves;
	std::unordered_map<robot_type, coords> robots_locations;

public:
	robot_map(int width_ = 1, int height_ = 1);

	// Getter
	int get_size() const;
	int get_height() const;
	int get_width() const;
	robot get_robot(int x, int y) const;
	bool is_grave(int x, int y) const;
	coords get_robots_location(robot_type type);

	// Setter
	void set_height(int height_);
	void set_width(int width_);
	void set_robot(const robot& robot_, int x, int y);
	void set_robots_grave(int x, int y);
	void move_robot(int x, int y, int new_x, int new_y);
};

/*
	The tile object is the object that is saved in the tile map so that the
	tiles can have their own functionality
*/
class tile {
protected:
	terrain tile_terrain;

public:
	int index;

	/* Properties important for dijkstra algorithmn */
	direction predecessor;
	int weights_nesw[4];
	bool is_in_heap;

	/* Constructor always checks, that passed Terrain-type is suitable for the
	child class, throws an exception if it is not and sets the tile_terrain
	after a successfull check */
	tile(terrain t = terrain::STEEL_PLANKS);
	tile(const tile&) = default;

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


class int_pair_comparator {
public:
	bool operator() (const int_pair& ip1, const int_pair& ip2);
};

/*
	Tile_map is implemented as one dimensional vector "i_map", where each
	coordinate-pair x and y can be mapped to its tile with y * width + x.
	Coordinates are starting from zero.
*/
class tile_map {
protected:
	int height;
	int width;
	bool has_pb_start;
	std::vector<std::shared_ptr<tile>> i_map;

public:
	std::string name;
	robot_map robots;

	tile_map(std::string _name = "No name.", int _width = 1, int _height = 1);

	// Getter :
	int get_size() const;
	int get_height() const;
	int get_width() const;
	terrain get_tile_terrain(int x, int y) const;
	std::shared_ptr<tile> get_tile(int x, int y);

	//Setter :
	void set_height(int h);
	void set_width(int w);
	void set_tile(const tile& t, int x, int y);
	void set_tile(const startingpoint& t, int x, int y);
	void set_tile(char c, int x, int y);

	// Setter for dijkstra 
	void init_map_graph_struct();
	/* Resets the values in tile, important for dijkstra but leaves the weights */
	void reset_all_tile_nodes();
	void update_adjacent_weights(int x, int y, int weight);
	void run_path_finding();
};