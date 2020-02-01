#pragma once
#include "pb_tiles.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

typedef std::pair<int, int> int_pair;

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
	/* Initializes the needed values for dijkstra in the tile classes and sets
	the edges od each tile/node */
	void init_map_graph_struct();
	/* Resets the values in tile, important for dijkstra but leaves the weights */
	void reset_all_tile_nodes();
	void update_adjacent_weights(int x, int y, int weight);
	void run_path_finding();
};