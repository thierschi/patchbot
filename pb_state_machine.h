#pragma once
#include "pb_map.h"

#include <map>
#include <queue>

class state_machine
{
public:
	state_machine(tile_map* map_ = nullptr, robot* self_ = nullptr);
	virtual ~state_machine() = default;

	virtual void process();

protected:
	tile_map* map;
	robot* self;

	coords target_tile;
	direction target_direction;
	bool wait;

	static const terrain free_tiles[];
	static const terrain walls[];

	virtual void initialize_machine();
};

class bugger_ki : public state_machine {
public:
	bugger_ki(tile_map* map_, robot* self_);
	~bugger_ki() override = default;

	void process() override;

private:
	const enum class state {FW, W, FaW };
	const enum class event { zb_u_ns, zf, zb, zf_u_ns, zs, zw, znw };

	state machines_state;
	coords start_tile;

	typedef void (bugger_ki::* state_func)();
	typedef bugger_ki::event(bugger_ki::* check_func)();
	typedef std::pair<state, event> state_event_pair;
	typedef std::pair<state, check_func> state_check_pair;

	std::map<state, check_func> event_getter;
	std::map<state_event_pair, state_func> transitions;

	void set_target_tile_FW();
	void set_target_tile_FaW();

	event get_event_at_FW();
	event get_event_at_W();
	event get_event_at_FAW();

	void to_state_FW();
	void to_state_W();
	void to_state_FAW();

	void initialize_machine() override;
};

class pushing_robot_ki : public state_machine {
public:
	pushing_robot_ki(tile_map* map_, robot* self_);
	~pushing_robot_ki() override = default;

	void process() override;

private:
	const enum class state { HB, VB, Z };
	const enum class event { zf, zw_o_pbb, zw_o_pbh, zg };

	state machines_state;

	typedef void (pushing_robot_ki::* state_func)();
	typedef pushing_robot_ki::event (pushing_robot_ki::* check_func)();
	typedef std::pair<state, event> state_event_pair;
	typedef std::pair<state, check_func> state_check_pair;

	std::map<state, check_func> event_getter;
	std::map<state_event_pair, state_func> transitions;

	void set_target_tile_HB();
	void set_target_tile_VB();

	event get_event_at_HB();
	event get_event_at_VB();

	void to_state_HB();
	void to_state_VB();

	void move_self();
	void push_robot();
	bool is_robot_blocked(const coords& pos, direction in_dir) const;

	void initialize_machine() override;
};

class aware_robot_ki : public state_machine {
public:
	aware_robot_ki(tile_map* map_, robot* self_);
	~aware_robot_ki() override = default;

	void process() override;

private:
	const enum class state { W, V, J };
	const enum class event { pe, pne };

	state machines_state;
	std::queue<direction> last_known_path;

	typedef void (aware_robot_ki::* state_func)();
	typedef std::pair<state, event> state_event_pair;

	std::map<state_event_pair, state_func> transitions;
	std::map<state_event_pair, state_func> transitions_hunter;

	event get_event() const;
	
	void to_state_V();
	void to_state_W();
	void to_state_J();

	void move_self(direction to);
	void save_path();

	void initialize_machine() override;
};