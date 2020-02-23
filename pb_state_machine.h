#pragma once
#include "pb_map.h"

#include <map>
#include <queue>
#include <memory>

/*
	Base class for all state_machines providing a basic template and some basic
	functionality, however fsm_tables are defined in the child_classes
*/
class state_machine
{
public:
	state_machine(tile_map* map_ = nullptr, std::shared_ptr<robot> self_ = nullptr);
	virtual ~state_machine() = default;

	/* Get id of robot that this fsm belonges to */
	unsigned int get_id() const;

	/* Get current event and transition to corresponding state, basically
	move the robot accordingly */
	virtual void process();

protected:
	tile_map* map;
	std::shared_ptr<robot> self;

	coords target_tile;
	direction target_direction;
	bool wait;

	/* Terrain of walls and of "free_tiles" saved here sperately because an
	interaction on the tile could result in unwanted effects (e.g. open door) */
	static const terrain free_tiles[];
	static const terrain walls[];

	/* Gets called in constructor, fills fsm_tables */
	virtual void initialize_machine();
};

class bugger_ki : public state_machine {
public:
	bugger_ki(tile_map* map_, std::shared_ptr<robot> self_);
	~bugger_ki() override = default;

	void process() override;

private:
	/* Abbreviations derived from german.
		FW = Follow wall; W = Wait; FaW = Find other/new wall;
	*/
	const enum class state {FW, W, FaW };
	/* Abbreviations derived from german.
		zb_u_ns = Target_tile is not free nor the start_tile; zf = target_tile free;
		zb = target_tile not free; zf_u_ns = Target_tile is free and not the start_tile;
		zs = target_tile is starting_tile; zw = target_tile is wall;
		zf_u_nw = target_tile is free and not a wall;
	*/
	const enum class event { zb_u_ns, zf, zb, zf_u_ns, zs, zw, znw };

	// Current state of fsm
	state machines_state;
	/* Tile after which bugger kept following a wall */
	coords start_tile;

	typedef void (bugger_ki::* state_func)();
	typedef bugger_ki::event(bugger_ki::* check_func)();
	typedef std::pair<state, event> state_event_pair;
	typedef std::pair<state, check_func> state_check_pair;

	/* Different check function for different states, because target_tiles differ
	and we also don't need to check for everything */
	std::map<state, check_func> event_getter;
	// Basically the fsm_table
	std::map<state_event_pair, state_func> transitions;

	void set_target_tile_FW();
	void set_target_tile_FaW();

	event get_event_at_FW();
	event get_event_at_W();
	event get_event_at_FaW();

	void to_state_FW();
	void to_state_W();
	void to_state_FAW();

	void initialize_machine() override;
};

class pushing_robot_ki : public state_machine {
public:
	pushing_robot_ki(tile_map* map_, std::shared_ptr<robot> self_);
	~pushing_robot_ki() override = default;

	void process() override;

private:
	/* Abbreviatoins derived from German.
		HB = Horizontal movement; VB = Vertical movement;
	*/
	const enum class state { HB, VB};
	/* Abbreviatoins derived from German.
		zf = target_tile free; zw_o_pbb = target_tile wall or patchbot's width;
		zw_o_pbh = target_tile wall or patchbot's width;
	*/
	const enum class event { zf, zw_o_pbb, zw_o_pbh};

	// Current state of machine
	state machines_state;

	typedef void (pushing_robot_ki::* state_func)();
	typedef pushing_robot_ki::event (pushing_robot_ki::* check_func)();
	typedef std::pair<state, event> state_event_pair;
	typedef std::pair<state, check_func> state_check_pair;

	/* Different check function for different states, because target_tiles differ
	and we also don't need to check for everything */
	std::map<state, check_func> event_getter;
	// Basically the fsm_table
	std::map<state_event_pair, state_func> transitions;

	void set_target_tile_HB();
	void set_target_tile_VB();

	event get_event_at_HB();
	event get_event_at_VB();

	void to_state_HB();
	void to_state_VB();

	void move_self();
	/* Pushes robot in target_direction if possible */
	void push_robot();
	bool is_robot_blocked(const coords& pos, direction in_dir) const;

	void initialize_machine() override;
};

class aware_robot_ki : public state_machine {
public:
	aware_robot_ki(tile_map* map_, std::shared_ptr<robot> self_);
	~aware_robot_ki() override = default;

	void process() override;

private:
	/* Abbreviations derived from German.
		W = Wait; V = Follow; J = Hunt;	
	*/
	const enum class state { W, V, J };
	/* Abbreviations derived from German.
		pe = Pathcbot reachable (or/and in sight); pne = Patchbot not reachable
		(or/and) in sight;
	*/
	const enum class event { pe, pne };

	// Current state of fsm
	state machines_state;
	/* Hunter saves the path to patchbot in case he gets out of sight, he will
	try to follow him nonetheless*/
	std::queue<direction> last_known_path;

	typedef void (aware_robot_ki::* state_func)();
	typedef std::pair<state, event> state_event_pair;

	/* Hunter has on state more, but regardless of state J the robots are
	extremly similar */
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