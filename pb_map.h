#pragma once
#include "pb_tiles.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

/*
	Robot_map is implemented as one dimensional vector  "__robot_map", where 
	each coordinate-pair x and y can be mapped to its tile with y * width + x.
	Coordinates are starting from zero.
*/
class robot_map {
protected:
	int height;
	int width;
	unsigned int pb_id;
	std::vector<std::shared_ptr<robot>> robots;
	std::unordered_map<int, int> graves;

public:
	/* Robots can push doors, that they've opened, in here to be processed
	and managed by the game_logic class */
	std::vector<coords> opened_doors;
	bool has_pb;
	std::unordered_map<unsigned int, coords> robots_locations;

	robot_map(int width_ = 1, int height_ = 1);

	// Getter
	int get_size() const;
	int get_height() const;
	int get_width() const;
	std::shared_ptr<robot> get_robot(int x, int y) const;
	std::shared_ptr<robot> get_robot(const coords& c) const;
	bool is_grave(int x, int y) const;
	coords get_patchbots_location() const;
	coords get_robots_location(unsigned int id) const;

	// Setter
	void set_height(int height_);
	void set_width(int width_);
	void set_robot(const robot& robot_, int x, int y);
	void set_robots_grave(int x, int y);
	void move_robot(int x, int y, int new_x, int new_y);
	void move_robot(unsigned int id, int new_x, int new_y);
	void kill_robot(unsigned int id);
	void update_graves();
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
	std::shared_ptr<tile> get_tile(int x, int y) const;
	bool is_in_line_of_sight(coords p1, coords p2) const;

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
	//void run_path_finding();
};