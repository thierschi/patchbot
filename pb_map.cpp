#pragma once
#include "pb_map.h"
#include "pb_exceptions.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>
#include <queue>
#include <functional>

/*
	Class robot_map
*/

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

robot robot_map::get_robot(const coords& c) const
{
	return get_robot(c.x, c.y);
}

bool robot_map::is_grave(int x, int y) const
{
	if (graves.find(y * width + x) == graves.end())
		return false;
	return true;
}

coords robot_map::get_patchbots_location() const
{
	return robots_locations.at(pb_id);
}

coords robot_map::get_robots_location(unsigned int id) const
{
	if(robots_locations.find(id) == robots_locations.end())
		throw std::invalid_argument("Invalid argument passed to robot_map: "
			"Passed robot does not exist.");
	return robots_locations.at(id);
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
	if (robot_.type == robot_type::PATCHBOT) {
		has_pb = true;
		pb_id = robot_.id;
	}
	robots_locations[robot_.id] = coords(x, y);
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
	robots[new_y * width + new_x] = std::move(robots[y * width + x]);
	robots[y * width + x] = robot();
	robots_locations[robots[new_y * width + new_x].id] = coords(new_x, new_y);
}

void robot_map::move_robot(unsigned int id, int new_x, int new_y)
{
	coords old = robots_locations.at(id);
	move_robot(old.x, old.y, new_x, new_y);
}

void robot_map::kill_robot(unsigned int id)
{
	coords r = robots_locations.at(id);
	robots_locations.erase(id);
	robots[r.y * width + r.x] = robot();

	if (graves.find(r.y * width + r.x) == graves.end())
		graves.insert(std::pair<int, int>(r.y * width + r.x, 5));
	else
		graves[r.y * width + r.x] = 5;
}

void robot_map::update_graves()
{
	for (std::pair<int, int> pos : graves) {
		if (pos.second == 0) {
			graves.erase(pos.first);
			continue;
		}
		graves[pos.first] = pos.second--;
	}
}




/*
	Class tile_map
*/

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

std::shared_ptr<tile> tile_map::get_tile(int x, int y) const {
	if (x >= width || y >= height)
		throw std::invalid_argument(
			"Invalid argument passed to Tile_map: "
			"Coordinates out of range.");
	return i_map[y * width + x];
}

bool tile_map::is_in_line_of_sight(coords p1, coords p2) const
{
	/* Return False if either one of the coordinates is out of bounds of the map */
	if (p1.x < 0 || p1.y < 0 || p2.x < 0 || p2.y < 0) return false;
	if (p1.x >= width || p1.y >= width 
		|| p2.x >= width || p2.y >= width) return false;
	/* Return true if the coordinates are the same */
	if (p1.x == p2.x && p1.y == p2.y) return true;

	// What is a wall?
	terrain walls[] = {
		terrain::CONCRETE_WALL,
		terrain::ROCK_WALL,
		terrain::MANUAL_DOOR,
		terrain::AUTOMATIC_DOOR,
		terrain::SECRET_PASSAGE
	};

	/* If start- or -endcoordinate is in the wall, return false */
	if (std::end(walls) != std::find(std::begin(walls), std::end(walls), 
		get_tile(p1.x, p1.y)->get_terrain())) return false;
	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		get_tile(p2.x, p2.y)->get_terrain())) return false;

	/*   -------- dx --------   --------- dy -------- */
	if (std::abs(p2.x - p1.x) > std::abs(p2.y - p1.y)) {
		if (p2.x < p1.x) {
			coords temp = p2;
			p2 = p1;
			p1 = temp;
		}

		/* Simple check, performed when p1 an p2 are on one line horizontally */
		if (p1.y == p2.y) {
			for (int i = p1.x + 1; i < p2.x; i++)
				if (std::end(walls) != std::find(
					std::begin(walls),
					std::end(walls), get_tile(i, p1.y)->get_terrain()))
					return false;
			return true;
		}
	}
	else {
		if (p2.y < p1.y) {
			coords temp = p2;
			p2 = p1;
			p1 = temp;
		}

		/* Simple check, performed when p1 an p2 are on one line vertiacally */
		if (p1.x == p2.x) {
			for (int i = p1.y + 1; i < p2.y; i++)
				if (std::end(walls) != std::find(
					std::begin(walls),
					std::end(walls), get_tile(p1.x, i)->get_terrain()))
					return false;
			return true;
		}
	}
	/* Diagonal is y = mx + t*/
	double m = (p2.y - p1.y) / (p2.x - p1.x);
	double t = p2.y - m * p2.x;

	if (std::abs(p2.x - p1.x) >= std::abs(p2.y - p1.y)) {
		/* Diagonale, solved for y */
		std::function<double(double)> diagonal = [&](double x) {
			return m * x + t;
		};

		for (double x = p1.x + 1; x <= p2.x; x += .5) {
			int y = diagonal(x);

			if (std::end(walls) != std::find(std::begin(walls),
				std::end(walls), get_tile(floor(x), 
					std::floor(y))->get_terrain()))
				return false;

			/* If y is even check the diagonal adjacent tiles too, because if 
			they are both a wall it is a non see-throught corner*/
			if (y == std::floor(y)) {
				if (std::end(walls) != std::find(std::begin(walls),
					std::end(walls), 
					get_tile(std::floor(x), std::floor(y) - 1)->get_terrain()))
					return false;
				if (std::end(walls) != std::find(std::begin(walls),
					std::end(walls), get_tile(std::floor(x) - 1, 
						std::floor(y))->get_terrain()))
					return false;
			}
		}
	}
	else {
		/* Diagonale, solved for x */
		std::function<double(double)> diagonal = [&](double y) {
			return (y - t) / m;
		};

		for (double y = p1.y + 1; y <= p2.y; y += .5) {
			int x = diagonal(y);

			if (std::end(walls) != std::find(std::begin(walls),
				std::end(walls), get_tile(std::floor(x), 
					std::floor(y))->get_terrain()))
				return false;
			
			/* If y is even check the diagonal adjacent tiles too, because if
			they are both a wall it is a non see-throught corner */
			if (x == std::floor(x)) {
				if (std::end(walls) != std::find(std::begin(walls),
					std::end(walls), get_tile(std::floor(x), 
						std::floor(y) - 1)->get_terrain()))
					return false;
				if (std::end(walls) != std::find(std::begin(walls),
					std::end(walls), get_tile(std::floor(x) - 1, 
						std::floor(y))->get_terrain()))
					return false;
			}
		}
	}
	return true;
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
	coords pb_pos = robots.get_patchbots_location();
	get_tile(pb_pos.x, pb_pos.y)->predecessor = direction::SOURCE;
}

void tile_map::reset_all_tile_nodes()
{
	for (int i = 0; i < i_map.size(); i++) {
			i_map[i]->predecessor = direction::UNDEFINED;
	}
	coords pb_pos = robots.get_patchbots_location();
	get_tile(pb_pos.x, pb_pos.y)->predecessor = direction::SOURCE;
}