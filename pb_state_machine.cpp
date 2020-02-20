#include "pb_state_machine.h"

const terrain abstract_state_machine::free_tiles[] = {
	terrain::ALIEN_GRASS,
	terrain::ENEMY_START,
	terrain::GRAVEL,
	terrain::OPEN_AUTOMATIC_DOOR,
	terrain::OPEN_MANUAL_DOOR,
	terrain::PATCHBOT_START,
	terrain::STEEL_PLANKS
};

const terrain abstract_state_machine::walls[] = {
	terrain::AUTOMATIC_DOOR,
	terrain::CONCRETE_WALL,
	terrain::MANUAL_DOOR,
	terrain::ROCK_WALL,
	terrain::SECRET_PASSAGE
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
	initialize_machine();
}

void bugger_ki::process()
{
}

bugger_ki::event bugger_ki::get_event_at_FW() const
{
	coords auround_self[] = {
		map->robots.get_robots_location(self->type),
		map->robots.get_robots_location(self->type),
		map->robots.get_robots_location(self->type),
		map->robots.get_robots_location(self->type)
	};

	return (bugger_ki::event)0;
}

bugger_ki::event bugger_ki::get_event_at_W() const
{
	return (bugger_ki::event)0;
}

bugger_ki::event bugger_ki::get_event_at_FAW() const
{
	return (bugger_ki::event)0;
}

void bugger_ki::to_state_FW()
{
}

void bugger_ki::to_state_W()
{
}

void bugger_ki::to_state_FAW()
{
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
	transitions[state_event_pair(state::FaW, event::zf_u_nw)] = &bugger_ki::to_state_FAW;
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

pusher_ki::event pusher_ki::get_event_at_HB() const
{
	return event::zf;
}

pusher_ki::event pusher_ki::get_event_at_VB() const
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