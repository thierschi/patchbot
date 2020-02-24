#include "pb_state_machine.h"

#include <algorithm>

/*
	class state_machine
*/

const terrain state_machine::free_tiles[] = {
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

const terrain state_machine::walls[] = { 
	terrain::CONCRETE_WALL,
	terrain::ROCK_WALL,
	terrain::SECRET_PASSAGE,
	terrain::MAIN_SERVER
};

state_machine::state_machine(tile_map* map_, const std::shared_ptr<robot>& self_) :
	map(map_),
	self(self_),
	target_direction(direction::UNDEFINED),
	wait(false)
{
}

unsigned int state_machine::get_id() const
{
	return self->id;
}

void state_machine::process()
{
}

void state_machine::initialize_machine()
{
}




/*
	class bugger_ki
*/

bugger_ki::bugger_ki(tile_map* map_, const std::shared_ptr<robot>& self_) :
	state_machine(map_, self_),
	machines_state(state::FW),
	start_tile(NULL)
{
	target_direction = direction::UNDEFINED;
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
	else
		/* Don't do anything when robot should wait */
		wait = false;
}

void bugger_ki::set_target_tile_FW()
{
	target_direction = direction::UNDEFINED;
	if (start_tile == NULL) {
		start_tile = map->robots.get_robots_location(self->id);
	}

	/* Get the four adjacent tiles, by getting own position 4 times and then
	manipulating them */
	coords around_self[] = {
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id),
		map->robots.get_robots_location(self->id)
	};

	/* If a tile is out of bounds of the map, it get's set to own tile and later
	treated as a wall, see *1 */
	around_self[0] = coords(around_self[0].x,
		std::max(around_self[0].y - 1, 0));
	around_self[1] = coords(std::min(around_self[1].x + 1, map->get_width() - 1),
		around_self[1].y);
	around_self[2] = coords(around_self[2].x,
		std::min(around_self[2].y + 1, map->get_height() - 1));
	around_self[3] = coords(std::max(around_self[3].x - 1, 0),
		around_self[3].y);

	/* Get next tile clockwise after wall */
	for (int i = 0; i < 4; i++) {
		terrain t;

		if (map->robots.get_robots_location(self->id) == around_self[i])
			/* *1 */
			t = terrain::CONCRETE_WALL;
		else
			t = map->get_tile(around_self[i].x, around_self[i].y)->get_terrain();

		if (t == terrain::AUTOMATIC_DOOR || t == terrain::MANUAL_DOOR)
			t = terrain::CONCRETE_WALL;

		if (std::find(std::begin(walls), std::end(walls), t) != std::end(walls)) {
			// If t is a wall
			if (!(around_self[(i + 1) % 4] == map->robots.get_robots_location(self->id)
				|| std::find(std::begin(walls), std::end(walls),
					map->get_tile(
						around_self[(i + 1) % 4].x, around_self[(i + 1) % 4].y)
					->get_terrain()) != std::end(walls))) {
				// If the next tile clockwise is not a wall too
				target_tile = around_self[(i + 1) % 4];
				break;
			}
		}
		else if (i == 3) {
			// If there is no wall around robot, choose a tile randomly
			target_tile = around_self[std::rand() % 4];
		}
	}
}

void bugger_ki::set_target_tile_FaW()
{
	start_tile = NULL;

	if (target_direction == direction::UNDEFINED) {
		/* If a direction is not yet determined (robot just entered state FaW) */
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

			/* Until here same as set_target_tile_FW */

			if (std::find(std::begin(walls), std::end(walls), t) != std::end(walls)) {
				/* If t is a wall, the target tile should be the tile on the
				opposite tile */
				target_tile = around_self[(i + 2) % 4];
				target_direction = (direction)((i + 2) % 4);
				break;
			}
		}
	}
	else {
		/* If a direction is deterrmined */

		// Get next tile in that direction
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
		// If target_tile is a free tile
		if (map->robots.get_robot(target_tile.x, target_tile.y)->type
			== robot_type::NONE)
			// And there is no robot occupying the tile
			return event::zf_u_ns;

	// Else the tile is blocked
	return event::zb_u_ns;
}

bugger_ki::event bugger_ki::get_event_at_W()
{
	if (std::find(std::begin(free_tiles), std::end(free_tiles),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain())
		!= std::end(free_tiles))
		// If target_tile is a free tile
		if (map->robots.get_robot(target_tile.x, target_tile.y)->type
			== robot_type::NONE)
			// And the tile is not occupied by a robot
			return event::zf;

	// Else the tile is blocked
	return event::zb;
}

bugger_ki::event bugger_ki::get_event_at_FaW() 
{
	set_target_tile_FaW();

	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain()))
		// If target tile is a wall
		return event::zw;

	return event::znw;
}

void bugger_ki::to_state_FW()
{
	machines_state = state::FW;
	/* Set target_tile again (for when state was switched) */
	set_target_tile_FW();
	
	// Interact with target_tile
	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id))->type);

	// Move robot
	map->robots.move_robot(self->id, target_tile.x, target_tile.y);

	// Kill robot or set wait if necesarry 
	switch (target_tiles_action) {
	case(action::DIE):
		map->robots.kill_robot(self->id);
		break;

	case(action::WALK_AND_WAIT):
		wait = true;

	default:
		break;
	}

	/* Note that bugger does not open doors in this state */
}

void bugger_ki::to_state_W()
{
	machines_state = state::W;

	// Do nothing
}

void bugger_ki::to_state_FAW()
{
	machines_state = state::FaW;
	/* Set target_tile again (for when state was switched) */
	set_target_tile_FaW();

	// Interact with target_tile
	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id))->type);

	// Move, kill set wait accordingly 
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
	event_getter[state::FaW] = &bugger_ki::get_event_at_FaW;

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

pushing_robot_ki::pushing_robot_ki(tile_map* map_, 
	const std::shared_ptr<robot>& self_) :
	state_machine(std::move(map_), self_),
	machines_state(state::HB)
{
	target_direction = direction::UNDEFINED;
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
	else
		/* Don't do anything when robot should wait */
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
	/* Get x position of patchbot to  check if patchbot's width is reached */
	int pb_x = map->robots.get_patchbots_location().x;

	if (pb_x == map->robots.get_robots_location(self->id).x)
		return event::zw_o_pbb;

	set_target_tile_HB();

	if (self->type == robot_type::SWIMMER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::WATER)
		// Check for water because the swimmer can, ..., well, swim
		return event::zf;

	if (self->type == robot_type::DIGGER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::ROCK_WALL)
		// Check for rock_walls, because the digger can dig
		return event::zf;

	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain()))
		// If target tile is wall
		return event::zw_o_pbb;

	if (is_robot_blocked(target_tile, target_direction))
		// Traget_tile is not wall, but there is a robot that is blocked
		return event::zw_o_pbb;

	return event::zf;
}

pushing_robot_ki::event pushing_robot_ki::get_event_at_VB()
{
	/* Get y position of patchbot to  check if patchbot's width is reached */
	int pb_y = map->robots.get_patchbots_location().y;

	if (pb_y == map->robots.get_robots_location(self->id).y)
		return event::zw_o_pbh;

	set_target_tile_VB();

	if (self->type == robot_type::SWIMMER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::WATER)
		// Check for water because the swimmer can, ..., well, swim
		return event::zf;

	if (self->type == robot_type::DIGGER && map->
		get_tile(target_tile.x, target_tile.y)->get_terrain() == terrain::ROCK_WALL)
		// Check for rock_walls, because the digger can dig
		return event::zf;

	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		map->get_tile(target_tile.x, target_tile.y)->get_terrain()))
		// If target tile is wall
		return event::zw_o_pbh;

	if (is_robot_blocked(target_tile, target_direction))
		// Traget_tile is not wall, but there is a robot that is blocked
		return event::zw_o_pbh;

	return event::zf;
}

void pushing_robot_ki::to_state_HB()
{
	if (machines_state != state::HB) {
		machines_state = state::HB;
		set_target_tile_HB();
	}

	// These actions are the same in HB and VB
	push_robot();
	move_self();
}

void pushing_robot_ki::to_state_VB()
{
	if (machines_state != state::VB) {
		machines_state = state::VB;
		set_target_tile_VB();
	}

	// These actions are the same in HB and VB
	push_robot();
	move_self();
}

void pushing_robot_ki::move_self()
{
	// Interct with target_tile
	action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
		->interact(map->robots.get_robot(
			map->robots.get_robots_location(self->id))->type);

	// Move, Die, Dig, etc. accordingly
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

	case(action::OBSTRUCTED):
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
	robot_type type = map->robots.get_robot(target_tile.x, target_tile.y)->type;

	// If there is no robot, return
	if (type == robot_type::NONE) return;
	if (is_robot_blocked(target_tile, target_direction)) return;

	// Determine pushed robot new position
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

	// Tile where robot gets pushed to may not be occupied
	if (map->robots.get_robot(robots_new_coords.x, robots_new_coords.y)->type
		!= robot_type::NONE) return;

	// Interact
	action robots_fate = map->get_tile(robots_new_coords.x, robots_new_coords.y)
		->interact(type);

	// Move the robot
	map->robots.move_robot(target_tile.x, target_tile.y,
		robots_new_coords.x, robots_new_coords.y);

	// Kill him if pushed into lethal tile
	if (robots_fate == action::DIE)
		map->robots.kill_robot(
			map->robots.get_robot(robots_new_coords.x, robots_new_coords.y)->id);
}

bool pushing_robot_ki::is_robot_blocked(const coords& pos, direction in_dir) const
{
	// If there is no robot on tile pos, it can't be blocked
	if (map->robots.get_robot(pos.x, pos.y)->type == robot_type::NONE)
		return false;

	// Get tile to check (next tile to robot in direction in_dir
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

	// If there is another robot, the checked robot cant be moved
	if (map->robots.get_robot(tile_to_check.x, tile_to_check.y)->type
		!= robot_type::NONE) return true;

	terrain tile_to_check_terrain =
		map->get_tile(tile_to_check.x, tile_to_check.y)->get_terrain();
	
	// If the robot is in front of a wall it is blocked
	if (std::end(walls) != std::find(std::begin(walls), std::end(walls),
		tile_to_check_terrain))
		return true;
	
	// A robot cannot be pushed though closed doors and they will not open
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




/*
	class aware_robot_ki
*/

aware_robot_ki::aware_robot_ki(tile_map* map_,
	const std::shared_ptr<robot>& self_) :
	state_machine(map_, self_),
	machines_state(state::W)
{
	initialize_machine();
}

void aware_robot_ki::process()
{
	if (!wait) {
		state_func s_f;

		// Use different fsm_table for hunter
		switch (self->type) {
		case (robot_type::HUNTER):
			s_f = transitions_hunter[state_event_pair(machines_state, get_event())];
			break;

		default:
			s_f = transitions[state_event_pair(machines_state, get_event())];
			break;
		}

		(this->*s_f)();
	}
	else
		/* Don't do anything when robot should wait */
		wait = false;
}

aware_robot_ki::event aware_robot_ki::get_event() const
{
	coords current_pos = map->robots.get_robots_location(self->id);
	if (self->type != robot_type::SNIFFER) {
		if (map->is_in_line_of_sight(map->robots.get_patchbots_location(),
			current_pos)
			&& map->get_tile(current_pos.x, current_pos.y)->predecessor
			!= direction::UNDEFINED) {
			return event::pe;
		}
	}
	else {
		// Sniffer doesn't care if he sees pathcbot
		if (map->get_tile(current_pos.x, current_pos.y)->predecessor
			!= direction::UNDEFINED)
			return event::pe;
	}
	return event::pne;
}

void aware_robot_ki::to_state_V()
{
	machines_state = state::V;
	
	// Move on fastest path to patchbit (in direction that is saved in tile)
	coords current_pos = map->robots.get_robots_location(self->id);
	move_self(map->get_tile(current_pos.x, current_pos.y)->predecessor);

	if (self->type == robot_type::HUNTER) {
		// Hunter moves twice
		if (!wait) {
			current_pos = map->robots.get_robots_location(self->id);
			move_self(map->get_tile(current_pos.x, current_pos.y)->predecessor);
		}
		else
			wait = false;
		save_path();
	}

}

void aware_robot_ki::to_state_W()
{
	machines_state = state::W;
	// Do nothing (wait)
}

void aware_robot_ki::to_state_J()
{
	machines_state = state::J;

	for (int i = 0; i < 2 && last_known_path.front() != direction::SOURCE; i++) {
		// Move twice on saved path to patchbot
		if (!wait) {
			move_self(last_known_path.front());
			last_known_path.pop();
		}
		else 
			wait = false;
	}
}

void aware_robot_ki::move_self(direction to)
{
	// Determine target tile based on given directoin "to"
	target_tile = map->robots.get_robots_location(self->id);
	switch (to) {
	case(direction::NORTH):
		target_tile.y--;
		break;

	case(direction::EAST):
		target_tile.x++;
		break;

	case(direction::SOUTH):
		target_tile.y++;
		break;

	case(direction::WEST):
		target_tile.x--;
		break;
	}

	/* Get robot_type on target tile, to determine if blocked or the game is lost
	because patchbot is there */
	robot_type on_target_tile = 
		map->robots.get_robot(target_tile.x, target_tile.y)->type;

	if (on_target_tile != robot_type::NONE) {
		if (on_target_tile == robot_type::PATCHBOT) {
			map->robots.has_pb = false;
			wait = true;
		}
	}
	else {
		// Interact with target tile
		action target_tiles_action = map->get_tile(target_tile.x, target_tile.y)
			->interact(map->robots.get_robot(
				map->robots.get_robots_location(self->id))->type);

		// Move, Die, Open door accordingly
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
}

void aware_robot_ki::save_path()
{
	// Reset queue
	last_known_path = std::queue<direction>();

	coords current_pos = map->robots.get_robots_location(self->id);

	bool is_end = false;
	while (!is_end) {
		/* Save predecessor of current tile an move to the next until reaching
		patchbot (direction::SOURCE) */
		direction predecessor =
			map->get_tile(current_pos.x, current_pos.y)->predecessor;
		
		last_known_path.push(predecessor);
		
		switch (predecessor) {
		case(direction::NORTH):
			current_pos.y--;
			break;

		case(direction::EAST):
			current_pos.x++;
			break;

		case(direction::SOUTH):
			current_pos.y++;
			break;

		case(direction::WEST):
			current_pos.x--;
			break;

		case (direction::SOURCE):
		default:
			is_end = true;
			break;
		}
	}
} 

void aware_robot_ki::initialize_machine()
{
	transitions[state_event_pair(state::W, event::pe)] = &aware_robot_ki::to_state_V;
	transitions[state_event_pair(state::W, event::pne)] = &aware_robot_ki::to_state_W;

	transitions[state_event_pair(state::V, event::pe)] = &aware_robot_ki::to_state_V;
	transitions[state_event_pair(state::V, event::pne)] = &aware_robot_ki::to_state_W;

	transitions_hunter[state_event_pair(state::W, event::pe)] = &aware_robot_ki::to_state_V;
	transitions_hunter[state_event_pair(state::W, event::pne)] = &aware_robot_ki::to_state_W;

	transitions_hunter[state_event_pair(state::V, event::pe)] = &aware_robot_ki::to_state_V;
	transitions_hunter[state_event_pair(state::V, event::pne)] = &aware_robot_ki::to_state_J;

	transitions_hunter[state_event_pair(state::J, event::pe)] = &aware_robot_ki::to_state_V;
	transitions_hunter[state_event_pair(state::J, event::pne)] = &aware_robot_ki::to_state_J;
}
