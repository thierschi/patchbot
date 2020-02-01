#pragma once
#include "pb_map.h"
#include "pb_exceptions.h"
#include "pb_img.h"

#include <iostream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <math.h>

const terrain dangers[] = {
	terrain::ABYSS,
	terrain::WATER
};

const terrain obstacles[] = {
	terrain::ALIEN_GRASS,
	terrain::GRAVEL,
	terrain::SECRET_PASSAGE
};

const terrain doors[] = {
	terrain::MANUAL_DOOR,
	terrain::AUTOMATIC_DOOR
};

const terrain walls[] = {
	terrain::CONCRETE_WALL,
	terrain::ROCK_WALL
};

const robot_type robots_with_wheels[] = {
	robot_type::PATCHBOT,
	robot_type::PUSHER,
	robot_type::DIGGER,
	robot_type::SWIMMER
};

coords::coords(int x_, int y_) :
	x(x_),
	y(y_)
{
}

robot::robot(robot_type type_, bool is_dead_) :
	type(type_),
	is_dead(is_dead_) 
{};

robot_map::robot_map(int width_, int height_) :
	width(width_),
	height(height_),
	has_pb(false)
{
	/*
		A new robot_map is always filled with robot_type NONE.
	*/
	robots.insert(robots.begin(), width * height, robot());
}

int robot_map::get_size() const {
	return width * height;
}

int robot_map::get_height() const {
	return height;
}

int robot_map::get_width() const {
	return width;
}

robot robot_map::get_robot(int x, int y) const {
	if (x >= width || y >= height)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Coordinates out of range.");
	return robots[y * width + x];
}

bool robot_map::is_grave(int x, int y) const
{
	if (graves.find(y * width + x) == graves.end())
		return false;
	return true;
}

coords robot_map::get_robots_location(robot_type type)
{
	if(robots_locations.find(type) == robots_locations.end())
		throw std::invalid_argument("Invalid argument passed to robot_map: "
			"Passed robot does not exist.");
	return robots_locations.at(type);
}

void robot_map::set_height(int height_) {
	/* Setting height by either appending
	width * new-height - old-height tiles
	or removing width * old-height - new-height tiles
	from the end */
	if (height_ <= 0)
		throw std::invalid_argument("Invalid argument passed to robot_map: "
			"Height of robot_map must be 1 or greater.");
	else if (height_ < height) {
		robots.erase(robots.begin() + height_ * width, 
			robots.end());
		height = height_;
	}
	else if (height_ > height) {
		robots.insert(robots.end(), (height_ - height) * width, 
			robot());
		height = height_;
	}
}

void robot_map::set_width(int width_) {
	/*
		Setting width by either removing the last
		new-width - old-with tiles in every section
		of old-width tiles or adding
		new-width - old-width new tiles
		every old-with times.
		In both cases starting from the back of the map.
	*/
	if (width_ <= 0)
		throw std::invalid_argument("Invalid argument passed to robot_map: "
			"Width of robot_map must be 1 or greater.");
	else if (width_ < width) {
		for (int i = height; i > 0; i--)
			robots.erase(robots.begin() + ((i - 1) * width) + width_, 
				robots.begin() + i * width);
		width = width_;
	}
	else if (width_ > width) {
		robots.push_back(robot());
		for (int i = height; i > 0; i--)
			robots.insert(robots.begin() + i * width, width_ - width, 
				robot());
		robots.pop_back();
		width = width_;
	}
}

void robot_map::set_robot(const robot& robot_, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Coordinates out of range.");
	if (robot_.type == robot_type::PATCHBOT && has_pb) 
		throw std::invalid_argument(
			"Invalid argument passed to robot_map: "
			"This robot_map already has a patchbot.");
	robots[y * width + x] = robot_;
	has_pb = (robot_.type == robot_type::PATCHBOT) ? true : has_pb;
	robots_locations[robot_.type] = coords(x, y);
}

void robot_map::set_robots_grave(int x, int y)
{
	if (x >= width || y >= height)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Coordinates out of range.");
	if (robots[y * width + x].type == robot_type::NONE)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Can't mark non existing robot as dead.");
	robots[y * width + x].is_dead = true;
	graves.insert(std::pair<int, bool>(y * width + x, true));
}

void robot_map::move_robot(int x, int y, int new_x, int new_y)
{

	if (x >= width || y >= height)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Coordinates out of range.");

	if (new_x >= width || new_y >= height)
		throw std::invalid_argument("Invalid argument passed to to robot_map: "
			"Coordinates out of range.");
	if (robots[y * width + x].type == robot_type::NONE)
		return;
	if (robots[new_y * width + new_x].type != robot_type::NONE)
		return;
	robots[new_y * width + new_x] = robots[y * width + x];
	robots[y * width + x] = robot();
	robots_locations[robots[new_y * width + new_x].type] = coords(new_x, new_y);
}

tile::tile(terrain t) :
	predecessor(direction::UNDEFINED),
	weights_nesw{0, 0, 0, 0},
	is_in_heap(false),
	tile_terrain(t)
{
	if (t != terrain::STEEL_PLANKS)
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
}


terrain tile::get_terrain() const {
	return tile_terrain;
}

action tile::interact(robot_type r) {
	return action::WALK;
}

int tile::get_weight()
{
	return 1;
}

startingpoint::startingpoint(robot_type r) :
	starting(r)
{
	if (r == robot_type::PATCHBOT) {
		tile_terrain = terrain::PATCHBOT_START;
		return;
	}
	if ((char)r < '1' || '7' < (char)r)
		// Check for the other robots (1-7)
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = terrain::ENEMY_START;
}

action startingpoint::interact(robot_type r)
{
	return action::WALK;
}

int startingpoint::get_weight()
{
	return 1;
}

danger::danger(terrain t)
{
	if (!(
		(std::find(std::begin(dangers),
			std::end(dangers),
			t)
			!= std::end(dangers))))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action danger::interact(robot_type r) {
	if (r == robot_type::SWIMMER
		&& tile_terrain
		== terrain::WATER)
		return action::WALK;
	return action::DIE;
}

int danger::get_weight()
{
	return 0;
}

obstacle::obstacle(terrain t) {
	if (!((std::find(std::begin(obstacles),
		std::end(obstacles),
		t)
		!= std::end(obstacles))))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action obstacle::interact(robot_type r) {
	if (tile_terrain == terrain::SECRET_PASSAGE)
		return (r == robot_type::PATCHBOT)
		? action::WALK : action::OBSTRUCTED;
	if ((std::find(std::begin(robots_with_wheels),
		std::end(robots_with_wheels),
		r)
		!= std::end(robots_with_wheels)))
		return (tile_terrain == terrain::ALIEN_GRASS)
		? action::WALK_AND_WAIT : action::WALK;
	return (tile_terrain == terrain::GRAVEL)
		? action::WALK_AND_WAIT : action::WALK;
}

int obstacle::get_weight()
{
	if (tile_terrain == terrain::ALIEN_GRASS)
		return 1;
	else if (tile_terrain == terrain::SECRET_PASSAGE)
		return 0;
	return 2;
}

door::door(terrain t) 
{
	if (!(std::find(std::begin(doors),
		std::end(doors),
		t)
		!= std::end(doors)))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action door::interact(robot_type r) {
	if (r == robot_type::NONE) {
		// Close door
		if (tile_terrain == terrain::OPEN_AUTOMATIC_DOOR) {
			tile_terrain = terrain::AUTOMATIC_DOOR;
			return action::WAIT;
		}
		else if (tile_terrain == terrain::OPEN_MANUAL_DOOR) {
			tile_terrain = terrain::MANUAL_DOOR;
			return action::WAIT;
		}
	}
	if (tile_terrain == terrain::OPEN_AUTOMATIC_DOOR ||
		tile_terrain == terrain::OPEN_MANUAL_DOOR)
		return action::WALK;
	if (r == robot_type::PATCHBOT) {
		if (tile_terrain == terrain::AUTOMATIC_DOOR)
			return action::OBSTRUCTED;
		tile_terrain = terrain::OPEN_MANUAL_DOOR;
		return action::OPEN_DOOR;
	}
	if (tile_terrain == terrain::AUTOMATIC_DOOR)
		tile_terrain = terrain::OPEN_AUTOMATIC_DOOR;
	else
		tile_terrain = terrain::OPEN_MANUAL_DOOR;
	return action::OPEN_DOOR;
}

int door::get_weight()
{
	return 0;
}

wall::wall(terrain t) {
	if (!((std::find(std::begin(walls),
		std::end(walls), t) != std::end(walls))))
		throw std::invalid_argument(
			"Invalid argument passed to "
			"constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action wall::interact(robot_type r) {
	if (r != robot_type::DIGGER)
		return action::OBSTRUCTED;
	return action::DIG;
}

int wall::get_weight()
{
	return 0;
}

server::server() {
	tile_terrain = terrain::MAIN_SERVER;
}

action server::interact(robot_type r) {
	if (r == robot_type::PATCHBOT)
		return action::WIN;
	return action::OBSTRUCTED;
}

int server::get_weight()
{
	return 0;
}

bool int_pair_comparator::operator()(const int_pair& ip1, const int_pair& ip2)
{
	return ip1.first < ip2.first;
}

tile_map::tile_map(std::string name_, int width_, int height_) :
	name(name_),
	width(width_),
	height(height_),
	has_pb_start(false),
	robots(robot_map(width_, height_))
{
	/* A new Tile_map is always filled with the standard
	Tile object more accurate with STEEL_PLANKS */
	i_map.insert(i_map.begin(), width_ * height_, std::make_shared<tile>());
}

// Getter
int tile_map::get_size() const {
	return width * height;
}

int tile_map::get_height() const {
	return height;
}

int tile_map::get_width()  const {
	return width;
}

terrain tile_map::get_tile_terrain(int x, int y) const {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	return i_map[y * width + x]->get_terrain();
}

std::shared_ptr<tile> tile_map::get_tile(int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	return i_map[y * width + x];
}

//Setter
void tile_map::set_height(int h) {
	/* Setting height by either appending
	width * new-height - old-height tiles
	or removing width * old-height - new-height tiles
	from the end */
	if (h <= 0) {
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Height of tile_map must be 1 or greater.");
	}
	else if (h < height) {
		i_map.erase(i_map.begin() + h * width, i_map.end());
		height = h;
	}
	else if (h > height) {
		i_map.insert(i_map.end(), (h - height) * width, 
			std::make_shared<tile>());
		height = h;
	}
	robots.set_height(h);
}

void tile_map::set_width(int w) {
	/*
		Setting width by either removing the last
		new-width - old-with tiles in every section
		of old-width tiles or adding
		new-width - old-width new tiles
		every old-with times.
		In both cases starting from the back of the map.
	*/
	if (w <= 0) {
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Width of tile_map must be 1 or greater.");
	}
	else if (w < width) {
		for (int i = height; i > 0; i--) {
			i_map.erase(i_map.begin()
				+ ((i - 1) * width) + w,
				i_map.begin() + i * width);
		}
		width = w;
	}
	else if (w > width) {
		i_map.push_back(std::make_shared<tile>());
		for (int i = height; i > 0; i--) {
			i_map.insert(i_map.begin() + i * width, w - width,
				std::make_shared<tile>());
		}
		i_map.pop_back();
		width = w;
	}
	robots.set_width(w);
}

void tile_map::set_tile(const tile& t, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	i_map[y * width + x] = std::make_shared<tile>(t);
	
}

void tile_map::set_tile(const startingpoint& t, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	if (t.starting == robot_type::PATCHBOT && has_pb_start)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"This map already has a startingpoint for patchbot.");

	i_map[y * width + x] = std::make_shared<tile>(t);
	robots.set_robot(robot(t.starting), x, y);
	has_pb_start = (t.starting == robot_type::PATCHBOT) ? true : has_pb_start;
}

void tile_map::set_tile(char c, int x, int y) {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");

	switch (c) {
	case 'p':
		if (has_pb_start)
			throw std::invalid_argument(
				"Invalid argument passed to Tile_map: "
				"This map already has a startingpoint for patchbot.");
		i_map[y * width + x] = std::make_shared<startingpoint>((robot_type)c);
		robots.set_robot(robot(robot_type::PATCHBOT), x, y);
		has_pb_start = true;
		break;
	case 'P':
		i_map[y * width + x] = std::make_shared<server>();
		break;
	case ' ':
		i_map[y * width + x] = std::make_shared<tile>();
		break;
	case '#':
		i_map[y * width + x] = std::make_shared<wall>(terrain::CONCRETE_WALL);
		break;
	case 'M':
		i_map[y * width + x] = std::make_shared<wall>(terrain::ROCK_WALL);
		break;
	case 'd':
		i_map[y * width + x] = std::make_shared<door>(terrain::MANUAL_DOOR);
		break;
	case 'D':
		i_map[y * width + x] = std::make_shared<door>(terrain::AUTOMATIC_DOOR);
		break;
	case 'g':
		i_map[y * width + x] = std::make_shared<obstacle>(terrain::ALIEN_GRASS);
		break;
	case '.':
		i_map[y * width + x] = std::make_shared<obstacle>(terrain::GRAVEL);
		break;
	case 'x':
		i_map[y * width + x] = std::make_shared<obstacle>(terrain::SECRET_PASSAGE);
		break;
	case 'O':
		i_map[y * width + x] = std::make_shared<danger>(terrain::ABYSS);
		break;
	case '~':
		i_map[y * width + x] = std::make_shared<danger>(terrain::WATER);
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		i_map[y * width + x] = std::make_shared<startingpoint>((robot_type)c);
		robots.set_robot(robot((robot_type)c), x, y);
		break;
	default:
		throw map_format_exception(
			"Map-format-exception: "
			"Unknown character in map.");
	}
}

void tile_map::init_map_graph_struct()
{
	for (int i = 0; i < i_map.size(); i++) {
		int x = i % width;
		int y = floor(i / width);

		/* NORTH */
		if (y == 0)
			i_map[i]->weights_nesw[0] = 0;
		else
			i_map[i]->weights_nesw[0] = get_tile(x, y - 1)->get_weight();

		/* EAST */
		if (x == width - 1)
			i_map[i]->weights_nesw[1] = 0;
		else
			i_map[i]->weights_nesw[1] = get_tile(x + 1, y)->get_weight();

		/* SOUTH */
		if (y == height - 1)
			i_map[i]->weights_nesw[2] = 0;
		else
			i_map[i]->weights_nesw[2] = get_tile(x, y + 1)->get_weight();

		/* WEST */
		if (x == 0)
			i_map[i]->weights_nesw[3] = 0;
		else
			i_map[i]->weights_nesw[3] = get_tile(x - 1, y)->get_weight();
	}
	coords pb_pos = robots.get_robots_location(robot_type::PATCHBOT);
	get_tile(pb_pos.x, pb_pos.y)->predecessor = direction::SOURCE;
}

void tile_map::reset_all_tile_nodes()
{
	for (int i = 0; i < i_map.size(); i++) {
			i_map[i]->predecessor = direction::UNDEFINED;
	}
	coords pb_pos = robots.get_robots_location(robot_type::PATCHBOT);
	get_tile(pb_pos.x, pb_pos.y)->predecessor = direction::SOURCE;
}

void tile_map::update_adjacent_weights(int x, int y, int weight)
{
	if (weight < 0)
		throw std::invalid_argument(
			"Invalid argument passed to tile_map:"
			"Edge's weight must be 0 or greater.");

	/*NORTH
		Set weight of south edge of notrthern tile, if exists*/
	if (y > 0)
		get_tile(x, y - 1)->weights_nesw[2] = weight;

	/*EAST
		Set weight of west edge of eastern tile, if exits*/
	if (x < width - 1)
		get_tile(x + 1, y)->weights_nesw[3] = weight;

	/*SOUTH
		Set weight of north edge of southern tile, if exits*/
	if (y < height - 1)
		get_tile(x, y + 1)->weights_nesw[0] = weight;

	/*WEST
		Set weight of east edge of western tile, if exits*/
	if (y > 0)
		get_tile(x - 1, y)->weights_nesw[1] = weight;
}

void tile_map::run_path_finding()
{
	std::priority_queue<int_pair, std::vector<int_pair>, int_pair_comparator> pq;

	std::vector<int> dist(i_map.size(), INT_MAX);

	coords pb_pos = robots.get_robots_location(robot_type::PATCHBOT);
	pq.push(std::make_pair(0, pb_pos.y * width + pb_pos.x));

	while (!pq.empty()) {
		int u = pq.top().second;
		pq.pop();

		/* NORTH */
		if (i_map[u]->weights_nesw[0] != 0 
			&& i_map[u]->predecessor != direction::NORTH) {
			if (dist[u - width] > dist[u] + i_map[u]->weights_nesw[0]) {
				dist[u - width] = dist[u] + i_map[u]->weights_nesw[0];
				pq.push(std::make_pair(dist[u - width], u - width));
				i_map[u - width]->predecessor = direction::SOUTH;
			}
		};

		/* EAST */
		if (i_map[u]->weights_nesw[1] != 0 
			&& i_map[u]->predecessor != direction::EAST) {
			if (dist[u + 1] > dist[u] + i_map[u]->weights_nesw[1]) {
				dist[u + 1] = dist[u] + i_map[u]->weights_nesw[1];
				pq.push(std::make_pair(dist[u + 1], u + 1));
				i_map[u + 1]->predecessor = direction::WEST;
			}
		};

		/* SOUTH */
		if (i_map[u]->weights_nesw[2] != 0
			&& i_map[u]->predecessor != direction::SOUTH) {
			if (dist[u + width] > dist[u] + i_map[u]->weights_nesw[2]) {
				dist[u + width] = dist[u] + i_map[u]->weights_nesw[2];
				pq.push(std::make_pair(dist[u + width], u + width));
				i_map[u + width]->predecessor = direction::NORTH;
			}
		};

		/* WEST */
		if (i_map[u]->weights_nesw[3] != 0
			&& i_map[u]->predecessor != direction::WEST) {
			if (dist[u - 1] > dist[u] + i_map[u]->weights_nesw[3]) {
				dist[u - 1] = dist[u] + i_map[u]->weights_nesw[3];
				pq.push(std::make_pair(dist[u - 1], u - 1));
				i_map[u - 1]->predecessor = direction::EAST;
			}
		};
		
	}
}