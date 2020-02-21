#include "pb_state_machine.h"
#include <algorithm>

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

const terrain abstract_state_machine::walls[] = { 
	terrain::MANUAL_DOOR,
	terrain::ROCK_WALL,
	terrain::SECRET_PASSAGE
};

abstract_state_machine::abstract_state_machine(tile_map* map_, robot* self_) :
	map(map_),
	self(self_)
{
}

pushing_robot_ki::pushing_robot_ki(tile_map* map_, robot* self_)
{
	map = map_;
	self = self_;
	machines_state = state::FW;
	start_tile = NULL;
	faw_direction = direction::UNDEFINED;
	initialize_machine();
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

void pushing_robot_ki::set_target_tile_FW()
{
	faw_direction = direction::UNDEFINED;
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

void pushing_robot_ki::set_target_tile_FaW()
{
	start_tile = NULL;

	if (faw_direction == direction::UNDEFINED) {
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

			if (map->robots.get_robots_location(self->id) == around_self[i]
				|| t == terrain::AUTOMATIC_DOOR || t == terrain::MANUAL_DOOR)
				t = terrain::CONCRETE_WALL;
			else
				t = map->get_tile(around_self[i].x, around_self[i].y)->get_terrain();

			if (std::find(std::begin(walls), std::end(walls), t) != std::end(walls)) {
				target_tile = around_self[(i + 2) % 4];
				break;
			}
		}
	}
	else {
		switch (faw_direction) {
		case(direction::NORTH):
			target_tile = coords(target_tile.x,
				std::max(0, target_tile.y - 1));
			break;

		case(direction::EAST):
			target_tile = coords(std::min(target_tile.x + 1, map->get_width() - 1),
				target_tile.y);
			break;

		case(direction::SOUTH):
			target_tile = coords(target_tile.x,
				std::max(target_tile.y + 1, map->get_height() - 1));
			break;

		case(direction::WEST):
			target_tile = coords(std::min(target_tile.x - 1, 0),
				target_tile.y);
		}
	}
}

pushing_robot_ki::event pushing_robot_ki::get_event_at_FW()
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

pushing_robot_ki::event pushing_robot_ki::get_event_at_W() 
{
	if (std::find(std::begin(free_tiles), std::end(free_tiles),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain())
		!= std::end(free_tiles))
		if (map->robots.get_robot(target_tile.x, target_tile.y).type
			== robot_type::NONE)
			return event::zf;

	return event::zb;
}

pushing_robot_ki::event pushing_robot_ki::get_event_at_FAW() 
{
	set_target_tile_FaW();

	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain()))
		if (map->robots.get_robot(target_tile.x, target_tile.y).type
			== robot_type::NONE)
		return event::zw;
	return event::znw;
}

void pushing_robot_ki::to_state_FW()
{
	machines_state = state::FW;
	set_target_tile_FW();
	
	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id)).type);

	switch (target_tiles_action) {
	case(action::DIE):
		map->robots.kill_robot(self->id);
		break;

	case(action::WALK_AND_WAIT):
		wait = true;

	default:
		map->robots.move_robot(self->id, target_tile.x, target_tile.y);
		break;
	}
}

void pushing_robot_ki::to_state_W()
{
	machines_state = state::W;
}

void pushing_robot_ki::to_state_FAW()
{
	machines_state = state::FaW;
	set_target_tile_FaW();

	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id)).type);

	switch (target_tiles_action) {
	case(action::DIE):
		map->robots.kill_robot(self->id);
		break;

	case(action::OPEN_DOOR):
		map->robots.opened_doors.push_back(target_tile);

	case(action::WALK_AND_WAIT):
		wait = true;

	default:
		map->robots.move_robot(self->id, target_tile.x, target_tile.y);
		break;
	}
}

void pushing_robot_ki::initialize_machine()
{
	event_getter[state::FW] = &pushing_robot_ki::get_event_at_FW;
	event_getter[state::W] = &pushing_robot_ki::get_event_at_W;
	event_getter[state::FaW] = &pushing_robot_ki::get_event_at_FAW;

	transitions[state_event_pair(state::FW, event::zb_u_ns)] = &pushing_robot_ki::to_state_W;
	transitions[state_event_pair(state::FW, event::zf_u_ns)] = &pushing_robot_ki::to_state_FW;
	transitions[state_event_pair(state::FW, event::zs)] = &pushing_robot_ki::to_state_FAW;

	transitions[state_event_pair(state::W, event::zf)] = &pushing_robot_ki::to_state_FW;
	transitions[state_event_pair(state::W, event::zb)] = &pushing_robot_ki::to_state_W;

	transitions[state_event_pair(state::FaW, event::zw)] = &pushing_robot_ki::to_state_FW;
	transitions[state_event_pair(state::FaW, event::znw)] = &pushing_robot_ki::to_state_FAW;
}

pusher_ki::pusher_ki(tile_map* map_, robot* self_)
{
	map = map_;
	self = self_;
	machines_state = state::HB;
	pusher_ki::initialize_machine();
}

void pusher_ki::process() 
{
}

pusher_ki::event pusher_ki::get_event_at_HB() 
{
	return event::zf;
}

pusher_ki::event pusher_ki::get_event_at_VB() 
{
	return event::zf;
}

void pusher_ki::to_state_HB()
{
}

void pusher_ki::to_state_VB()
{
}

void pusher_ki::to_state_Z()
{
}

void pusher_ki::initialize_machine()
{
	event_getter[state::HB] = &pusher_ki::get_event_at_HB;
	event_getter[state::VB] = &pusher_ki::get_event_at_VB;

	transitions[state_event_pair(state::HB, event::zf)] = &pusher_ki::to_state_HB;
	transitions[state_event_pair(state::HB, event::zw_o_pbb)] = &pusher_ki::to_state_VB;
	transitions[state_event_pair(state::HB, event::zg)] = &pusher_ki::to_state_Z;

	transitions[state_event_pair(state::VB, event::zf)] = &pusher_ki::to_state_VB;
	transitions[state_event_pair(state::VB, event::zw_o_pbh)] = &pusher_ki::to_state_HB;
	transitions[state_event_pair(state::VB, event::zg)] = &pusher_ki::to_state_Z;
}