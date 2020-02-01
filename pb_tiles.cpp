#include "pb_tiles.h"

#include <stdexcept>

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
{}

robot::robot(robot_type type_, bool is_dead_) :
	type(type_),
	is_dead(is_dead_)
{}

tile::tile(terrain t) :
	predecessor(direction::UNDEFINED),
	weights_nesw{ -1, -1, -1, -1},
	tile_terrain(t)
{
	if (t != terrain::STEEL_PLANKS)
		throw std::invalid_argument(
			"Invalid argument passed to constructor of tile: "
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
			"Invalid argument passed to constructor of tile: "
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
	if (!((std::find(std::begin(dangers), std::end(dangers), t) 
			!= std::end(dangers))))
		/* Check if passed terrain is indeed a danger */
		throw std::invalid_argument(
			"Invalid argument passed to constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action danger::interact(robot_type r) {
	if (r == robot_type::SWIMMER && tile_terrain == terrain::WATER)
		return action::WALK;
	return action::DIE;
}

int danger::get_weight()
{
	return -1;
}

obstacle::obstacle(terrain t) {
	if (!((std::find(std::begin(obstacles), std::end(obstacles), t)
		!= std::end(obstacles))))
		/* Check if passed terrain is indeed an obstacle */
		throw std::invalid_argument(
			"Invalid argument passed to constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action obstacle::interact(robot_type r) {
	if (tile_terrain == terrain::SECRET_PASSAGE)
		/* Patchbot can walk on secret passages */
		return (r == robot_type::PATCHBOT) ? action::WALK : action::OBSTRUCTED;

	if ((std::find(std::begin(robots_with_wheels), 
		std::end(robots_with_wheels), r) != std::end(robots_with_wheels)))
		/* Robots with wheels get slowed down by alien grass */
		return (tile_terrain == terrain::ALIEN_GRASS) 
		? action::WALK_AND_WAIT : action::WALK;

	/* Robots with legs get slowed down by gravel */
	return (tile_terrain == terrain::GRAVEL)
		? action::WALK_AND_WAIT : action::WALK;
}

int obstacle::get_weight()
{
	if (tile_terrain == terrain::ALIEN_GRASS)
		return 1;
	else if (tile_terrain == terrain::SECRET_PASSAGE)
		return -1;
	return 2;
}

door::door(terrain t)
{
	if (!(std::find(std::begin(doors), std::end(doors), t)
		!= std::end(doors)))
		/* Check that passed terrain is indeed a door */
		throw std::invalid_argument(
			"Invalid argument passed to constructor of tile: "
			"Terrain class missmatch.");
	tile_terrain = t;
}

action door::interact(robot_type r) {
	if (r == robot_type::NONE) {
		/* Close door (the texture at least) => closing mechanismns are 
		handeled seperately (s. pb_game_logic.h) */
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
		/* Walk on door if it is open */
		return action::WALK;

	if (r == robot_type::PATCHBOT) {
		/* Patchbot gets obstructed by automatic doors and opens manuel doors*/
		if (tile_terrain == terrain::AUTOMATIC_DOOR)
			return action::OBSTRUCTED;
		tile_terrain = terrain::OPEN_MANUAL_DOOR;
		return action::OPEN_DOOR;
	}

	/* The other robots can open every door */
	if (tile_terrain == terrain::AUTOMATIC_DOOR)
		tile_terrain = terrain::OPEN_AUTOMATIC_DOOR;
	else
		tile_terrain = terrain::OPEN_MANUAL_DOOR;
	return action::OPEN_DOOR;
}

int door::get_weight()
{
	return 2;
}

wall::wall(terrain t) {
	if (!((std::find(std::begin(walls), std::end(walls), t) != std::end(walls))))
		/* Check if passed terrain is indeed a wall */
		throw std::invalid_argument(
			"Invalid argument passed to constructor of tile: "
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
	return -1;
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
	return -1;
}