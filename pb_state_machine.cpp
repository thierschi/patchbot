#include "pb_state_machine.h"
#include <algorithm>

/*
	class abstract_state_machine
*/

const terrain abstract_state_machine::free_tiles[] = {
	terrain::ALIEN_GRASS,
	terrain::ENEMY_START,
	terrain::GRAVEL,
	terrain::OPEN_AUTOMATIC_DOOR,
	terrain::OPEN_MANUAL_DOOR,
	terrain::PATCHBOT_START,
	terrain::STEEL_PLANKS,
	terrain::ABYSS,
	terrain::WATER
};




/*
	class bugger_ki
*/

const terrain abstract_state_machine::walls[] = { 
	terrain::CONCRETE_WALL,
	terrain::ROCK_WALL,
	terrain::SECRET_PASSAGE,
	terrain::MAIN_SERVER
};

abstract_state_machine::abstract_state_machine(tile_map* map_, robot* self_) :
	map(map_),
	self(self_)
{
}

bugger_ki::bugger_ki(tile_map* map_, robot* self_)
{
	map = map_;
	self = self_;
	machines_state = state::FW;
	start_tile = NULL;
	faw_direction = direction::UNDEFINED;
	initialize_machine();
}

void bugger_ki::process()
{
	if (!wait) {
		check_func c_f = event_getter[machines_state];
		state_func s_f = transitions[state_event_pair(
			machines_state, (this->*c_f)())];
		(this->*s_f)();
	}
	wait = false;
}

void bugger_ki::set_target_tile_FW()
{
	target_direction = direction::UNDEFINED;
	if (start_tile == NULL) {
		start_tile = map->robots.get_robots_location(self->id);
	}

	coords around_self[] = {
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id)
	};

	around_self[0] = coords(around_self[0].x,
		std::max(around_self[0].y - 1, 0));
	around_self[1] = coords(std::min(around_self[1].x + 1, map->get_width() - 1),
		around_self[1].y);
	around_self[2] = coords(around_self[2].x,
		std::min(around_self[2].y + 1, map->get_height() - 1));
	around_self[3] = coords(std::max(around_self[3].x - 1, 0),
		around_self[3].y);

	for (int i = 0; i < 4; i++) {
		terrain t;

		if (map->robots.get_robots_location(self->id) == around_self[i])
			t = terrain::CONCRETE_WALL;
		else
			t = map->get_tile(around_self[i].x, around_self[i].y)->get_terrain();

		if (std::find(std::begin(walls), std::end(walls), t) != std::end(walls)) {
			if (!(around_self[(i + 1) % 4] == map->robots.get_robots_location(self->id)
				|| std::find(std::begin(walls), std::end(walls),
					map->get_tile(
						around_self[(i + 1) % 4].x, around_self[(i + 1) % 4].y)
					->get_terrain()) != std::end(walls))) {
				target_tile = around_self[(i + 1) % 4];
				break;
			}
		}
		else if (i - 1 == 3) {
			target_tile = around_self[self->id % 4];
		}
	}
}

void bugger_ki::set_target_tile_FaW()
{
	start_tile = NULL;

	if (target_direction == direction::UNDEFINED) {
		coords around_self[] = {
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id)
		};

		around_self[0] = coords(around_self[0].x,
			std::max(around_self[0].y - 1, 0));
		around_self[1] = coords(std::min(around_self[1].x + 1, map->get_width() - 1),
			around_self[1].y);
		around_self[2] = coords(around_self[2].x,
			std::min(around_self[2].y + 1, map->get_height() - 1));
		around_self[3] = coords(std::max(around_self[3].x - 1, 0),
			around_self[3].y);

		for (int i = 0; i < 4; i++) {
			terrain t;

			if (map->robots.get_robots_location(self->id) == around_self[i])
				t = terrain::CONCRETE_WALL;
			else
				t = map->get_tile(around_self[i].x, around_self[i].y)->get_terrain();

			if (t == terrain::AUTOMATIC_DOOR || t == terrain::MANUAL_DOOR)
				t = terrain::CONCRETE_WALL;

			if (std::find(std::begin(walls), std::end(walls), t) != std::end(walls)) {
				target_tile = around_self[(i + 2) % 4];
				target_direction = (direction)((i + 2) % 4);
				break;
			}
		}
	}
	else {
		coords current_position = map->robots.get_robots_location(self->id);
		switch (target_direction) {
		case(direction::NORTH):
			target_tile = coords(current_position.x,
				std::max(0, current_position.y - 1));
			break;

		case(direction::EAST):
			target_tile = coords(std::min(current_position.x + 1, 
				map->get_width() - 1), current_position.y);
			break;

		case(direction::SOUTH):
			target_tile = coords(current_position.x,
				std::max(current_position.y + 1, map->get_height() - 1));
			break;

		case(direction::WEST):
			target_tile = coords(std::min(current_position.x - 1, 0),
				current_position.y);
		}
	}
}

bugger_ki::event bugger_ki::get_event_at_FW()
{
	set_target_tile_FW();

	if (target_tile == start_tile)
		return event::zs;

	if (std::find(std::begin(free_tiles), std::end(free_tiles),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain())
		!= std::end(free_tiles))
		if (map->robots.get_robot(target_tile.x, target_tile.y).type
			== robot_type::NONE)
			return event::zf_u_ns;

	return event::zb_u_ns;
}

bugger_ki::event bugger_ki::get_event_at_W() 
{
	if (std::find(std::begin(free_tiles), std::end(free_tiles),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain())
		!= std::end(free_tiles))
		if (map->robots.get_robot(target_tile.x, target_tile.y).type
			== robot_type::NONE)
			return event::zf;

	return event::zb;
}

bugger_ki::event bugger_ki::get_event_at_FAW() 
{
	set_target_tile_FaW();

	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain()))
		if (map->robots.get_robot(target_tile.x, target_tile.y).type
			== robot_type::NONE)
		return event::zw;
	return event::znw;
}

void bugger_ki::to_state_FW()
{
	machines_state = state::FW;
	set_target_tile_FW();
	
	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id)).type);

	map->robots.move_robot(self->id, target_tile.x, target_tile.y);

	switch (target_tiles_action) {
	case(action::DIE):
		map->robots.kill_robot(self->id);
		break;

	case(action::WALK_AND_WAIT):
		wait = true;

	default:
		break;
	}
}

void bugger_ki::to_state_W()
{
	machines_state = state::W;
}

void bugger_ki::to_state_FAW()
{
	machines_state = state::FaW;
	set_target_tile_FaW();

	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id)).type);


	switch (target_tiles_action) {
	case(action::DIE):
		map->robots.move_robot(self->id, target_tile.x, target_tile.y);
		map->robots.kill_robot(self->id);
		break;

	case(action::OPEN_DOOR):
		map->robots.opened_doors.push_back(target_tile);
		break;

	case(action::WALK_AND_WAIT):
		wait = true;

	default:
		map->robots.move_robot(self->id, target_tile.x, target_tile.y);
		break;
	}
}

void bugger_ki::initialize_machine()
{
	event_getter[state::FW] = &bugger_ki::get_event_at_FW;
	event_getter[state::W] = &bugger_ki::get_event_at_W;
	event_getter[state::FaW] = &bugger_ki::get_event_at_FAW;

	transitions[state_event_pair(state::FW, event::zb_u_ns)] = &bugger_ki::to_state_W;
	transitions[state_event_pair(state::FW, event::zf_u_ns)] = &bugger_ki::to_state_FW;
	transitions[state_event_pair(state::FW, event::zs)] = &bugger_ki::to_state_FAW;

	transitions[state_event_pair(state::W, event::zf)] = &bugger_ki::to_state_FW;
	transitions[state_event_pair(state::W, event::zb)] = &bugger_ki::to_state_W;

	transitions[state_event_pair(state::FaW, event::zw)] = &bugger_ki::to_state_FW;
	transitions[state_event_pair(state::FaW, event::znw)] = &bugger_ki::to_state_FAW;
}




/*
	class pushing_robot_ki
*/

pushing_robot_ki::pushing_robot_ki(tile_map* map_, robot* self_)
{
	map = map_;
	self = self_;
	machines_state = state::HB;
	pushing_robot_ki::initialize_machine();
}

void pushing_robot_ki::process() 
{
	if (!wait) {
		check_func c_f = event_getter[machines_state];
		state_func s_f = transitions[state_event_pair(
			machines_state, (this->*c_f)())];
		(this->*s_f)();
	}
	wait = false;
}

void pushing_robot_ki::set_target_tile_HB()
{
	coords current_position = map->robots.get_robots_location(self->id);
	target_direction = 
		(map->robots.get_patchbots_location().x > current_position.x) 
		? direction::EAST : direction::WEST;
	target_tile = coords(
		(map->robots.get_patchbots_location().x > current_position.x) ?
		current_position.x + 1 : current_position.x - 1, current_position.y);
}

void pushing_robot_ki::set_target_tile_VB()
{
	coords current_position = map->robots.get_robots_location(self->id);
	target_direction =
		(map->robots.get_patchbots_location().y > current_position.y)
		? direction::SOUTH : direction::NORTH;
	target_tile = coords(
		current_position.x,
		(map->robots.get_patchbots_location().y > current_position.y)
		? current_position.y + 1 : current_position.y - 1);
}

pushing_robot_ki::event pushing_robot_ki::get_event_at_HB() 
{
	int pb_x = map->robots.get_patchbots_location().x;

	if (pb_x == map->robots.get_robots_location(self->id).x)
		return event::zw_o_pbb;

	set_target_tile_HB();

	if (self->type == robot_type::SWIMMER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::WATER)
		return event::zf;

	if (self->type == robot_type::DIGGER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::ROCK_WALL)
		return event::zf;

	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain()))
		return event::zw_o_pbb;

	if (is_robot_blocked(target_tile, target_direction))
		return event::zw_o_pbb;

	return event::zf;
}

pushing_robot_ki::event pushing_robot_ki::get_event_at_VB()
{
	int pb_y = map->robots.get_patchbots_location().x;

	if (pb_y == map->robots.get_robots_location(self->id).y)
		return event::zw_o_pbb;

	set_target_tile_VB();

	if (self->type == robot_type::SWIMMER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::WATER)
		return event::zf;

	if (self->type == robot_type::DIGGER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::ROCK_WALL)
		return event::zf;

	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain()))
		return event::zw_o_pbb;

	if (is_robot_blocked(target_tile, target_direction))
		return event::zw_o_pbb;

	return event::zf;
}

void pushing_robot_ki::to_state_HB()
{
	if (machines_state != state::HB) {
		machines_state = state::HB;
		set_target_tile_HB();
	}

	push_robot();
	move_self();
}

void pushing_robot_ki::to_state_VB()
{
	if (machines_state != state::VB) {
		machines_state = state::VB;
		set_target_tile_VB();
	}

	push_robot();
	move_self();
}

void pushing_robot_ki::move_self()
{
	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id)).type);

	switch (target_tiles_action) {
	case(action::DIE):
		map->robots.move_robot(self->id, target_tile.x, target_tile.y);
		map->robots.kill_robot(self->id);
		break;

	case(action::OPEN_DOOR):
		map->robots.opened_doors.push_back(target_tile);
		break;

	case(action::DIG):
		map->set_tile((char)terrain::GRAVEL, target_tile.x, target_tile.y);
		map->robots.move_robot(self->id, target_tile.x, target_tile.y);
		break;

	case(action::WALK_AND_WAIT):
		wait = true;

	default:
		map->robots.move_robot(self->id, target_tile.x, target_tile.y);
		break;
	}
}

void pushing_robot_ki::push_robot()
{
	robot_type type = map->robots.get_robot(target_tile.x, target_tile.y).type;
	
	if (type == robot_type::NONE) return;

	coords robots_new_coords = target_tile;
	switch (target_direction) {
	case(direction::NORTH):
		robots_new_coords.y--;
		break;

	case(direction::EAST):
		robots_new_coords.x++;
		break;

	case(direction::SOUTH):
		robots_new_coords.y++;
		break;

	case(direction::WEST):
		robots_new_coords.x--;
		break;
		
	default:
		return;
	}

	if (map->robots.get_robot(robots_new_coords.x, robots_new_coords.y).type
		!= robot_type::NONE) return;

	action robots_fate = map->get_tile(robots_new_coords.x, robots_new_coords.y)
		->interact(type);

	map->robots.move_robot(target_tile.x, target_tile.y,
		robots_new_coords.x, robots_new_coords.y);

	if (robots_fate == action::DIE)
		map->robots.kill_robot(
			map->robots.get_robot(target_tile.x, target_tile.y).id);
}

bool pushing_robot_ki::is_robot_blocked(const coords& pos, direction in_dir) const
{
	if (map->robots.get_robot(pos.x, pos.y).type == robot_type::NONE)
		return false;

	coords tile_to_check = pos;
	switch (in_dir) {
	case(direction::NORTH):
		tile_to_check.y--;
		break;

	case(direction::EAST):
		tile_to_check.x++;
		break;

	case(direction::SOUTH):
		tile_to_check.y++;
		break;

	case(direction::WEST):
		tile_to_check.x--;
		break;
	default:
		return true;
	}

	if (map->robots.get_robot(tile_to_check.x, tile_to_check.y).type
		!= robot_type::NONE) return true;

	terrain tile_to_check_terrain =
		map->get_tile(tile_to_check.x, tile_to_check.y)->get_terrain();
	
	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		tile_to_check_terrain))
		return true;
	
	if (tile_to_check_terrain == terrain::AUTOMATIC_DOOR
		|| tile_to_check_terrain == terrain::MANUAL_DOOR)
		return true;

	return false;
}

void pushing_robot_ki::initialize_machine()
{
	event_getter[state::HB] = &pushing_robot_ki::get_event_at_HB;
	event_getter[state::VB] = &pushing_robot_ki::get_event_at_VB;

	transitions[state_event_pair(state::HB, event::zf)] = &pushing_robot_ki::to_state_HB;
	transitions[state_event_pair(state::HB, event::zw_o_pbb)] = &pushing_robot_ki::to_state_VB;

	transitions[state_event_pair(state::VB, event::zf)] = &pushing_robot_ki::to_state_VB;
	transitions[state_event_pair(state::VB, event::zw_o_pbh)] = &pushing_robot_ki::to_state_HB;
}