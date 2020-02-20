#pragma once
#include "pb_map.h"

#include <map>
#include <functional>

class abstract_state_machine
{
public:
	abstract_state_machine() = default;
	abstract_state_machine(tile_map* map_, robot* self_);
	virtual ~abstract_state_machine() = default;

	virtual void process() = 0;

protected:
	tile_map* map;
	robot* self;

	static const terrain free_tiles[];
	static const terrain walls[];

	virtual void initialize_machine() = 0;
};

class bugger_ki : public abstract_state_machine {
public:
	bugger_ki(tile_map* map_, robot* self_);
	~bugger_ki() override = default;

	void process() override;

private:
	const enum class state {FW, W, FaW };
	const enum class event { zb_u_ns, zf, zb, zf_u_ns, zs, zw, zf_u_nw };

	state machines_state;
	coords target_tile;

	typedef void (bugger_ki::* state_func)();
	typedef bugger_ki::event(bugger_ki::* check_func)() const;
	typedef std::pair<state, event> state_event_pair;
	typedef std::pair<state, check_func> state_check_pair;

	std::map<state, check_func> event_getter;
	std::map<state_event_pair, state_func> transitions;

	event get_event_at_FW() const;
	event get_event_at_W() const;
	event get_event_at_FAW() const;

	void to_state_FW();
	void to_state_W();
	void to_state_FAW();

	void initialize_machine() override;
};

class pusher_ki : public abstract_state_machine {
public:
	pusher_ki(tile_map* map_, robot* self_);
	~pusher_ki() override = default;

	void process();

private:
	const enum class state { HB, VB, Z };
	const enum class event { zf, zw_o_pbb, zw_o_pbh, zg };

	state machines_state;
	coords target_tile;

	typedef void (pusher_ki::* state_func)();
	typedef pusher_ki::event (pusher_ki::* check_func)() const;
	typedef std::pair<state, event> state_event_pair;
	typedef std::pair<state, check_func> state_check_pair;

	std::map<state, check_func> event_getter;
	std::map<state_event_pair, state_func> transitions;

	event get_event_at_HB() const;
	event get_event_at_VB() const;

	void to_state_HB();
	void to_state_VB();
	void to_state_Z();

	void initialize_machine() override;
};

