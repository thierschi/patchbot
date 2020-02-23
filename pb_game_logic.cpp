#include "pb_game_logic.h"
#include "pb_map.h"
#include "pb_path_finding.h"

#include <chrono>
#include <thread>
#include <QMessageBox>
#include <stack>

open_door::open_door(int x_, int y_, int open_since_) :
	x(x_),
	y(y_),
	open_since(open_since_)
{
}

game_logic::game_logic(main_window* parent_,
	rendering_engine* rendering_engine_, controls* controls_) :
	parent(parent_),
	p_rendering_engine(rendering_engine_),
	p_controls(controls_),
	automation_timer(new QTimer(this)),
	duration(10)
{
	connect_to_parent();
	QObject::connect(automation_timer, &QTimer::timeout,
		this, &game_logic::single_step);
}

game_logic::~game_logic() {
	delete automation_timer;
}

void game_logic::connect_to_parent()
{
	QObject::connect(parent, &main_window::start_game,
		this, &game_logic::start_game);
	QObject::connect(parent, &main_window::single_step,
		this, &game_logic::single_step);
	QObject::connect(parent, &main_window::automatic_stepping,
		this, &game_logic::automatic_stepping);
	QObject::connect(parent, &main_window::stop_automatic_stepping,
		this, &game_logic::stop_automatic_stepping);
	QObject::connect(parent, &main_window::cancel_game,
		this, &game_logic::cancel_game);
}

void game_logic::initialize_enemy_kis()
{
	enemy_kis.clear();

	for (std::pair<unsigned int, coords> r : parent->map.robots.robots_locations) {
		std::shared_ptr<robot> robot_ = parent->map.robots.get_robot(r.second);

		switch (robot_->type) {
		case(robot_type::BUGGER):
			enemy_kis.push_back(std::make_unique<bugger_ki>(&parent->map, robot_));
			break;

		case(robot_type::PUSHER):
		case(robot_type::DIGGER):
		case(robot_type::SWIMMER):
			enemy_kis.push_back(std::make_unique<pushing_robot_ki>(&parent->map, robot_));
			break;

		case(robot_type::FOLLOWER):
		case(robot_type::HUNTER):
		case(robot_type::SNIFFER):
			enemy_kis.push_back(std::make_unique<aware_robot_ki>(&parent->map, robot_));
			break;
		}
	}
}

void game_logic::move_patchbot()
{
	// If no instructions left -> Lost
	if (p_controls->instruction_queue.size() < 1) {
		loose_game();
		return;
	}

	// Get next instruction
	instruction_type current_instruction = p_controls->
		instruction_queue.front().type;
	if (p_controls->instruction_queue.front().amount == 1)
		// Delete if amount is 1
		p_controls->instruction_queue.erase(
			p_controls->instruction_queue.begin());
	else if (p_controls->instruction_queue.front().amount > 1)
		// Decrement amount
		p_controls->instruction_queue.front().amount--;

	// Create old and new coords
	coords old_pos = parent->
		map.robots.get_patchbots_location( );
	coords new_pos = coords();

	action patchbot_action;

	// Get new coords if patchbot would walk
	switch (current_instruction)
	{
	case(instruction_type::UP):
		new_pos.x = old_pos.x;

		if (old_pos.y - 1 < 0) {
			// When on the edge of the map
			patchbot_action = action::OBSTRUCTED;
			new_pos.y = old_pos.y;
		}
		else {
			// Set new coordinates one step up
			patchbot_action = action::WALK;
			new_pos.y = old_pos.y - 1;
		}
		break;

	case(instruction_type::DOWN):
		new_pos.x = old_pos.x;

		if (old_pos.y + 1 >= parent->map.get_height()) {
			// When on the edge of the map
			patchbot_action = action::OBSTRUCTED;
			new_pos.y = old_pos.y;
		}
		else {
			// Set new coordinates one step down
			patchbot_action = action::WALK;
			new_pos.y = old_pos.y + 1;
		}
		break;

	case(instruction_type::LEFT):
		new_pos.y = old_pos.y;

		if (old_pos.x - 1 < 0) {
			// When on the edge of the map
			patchbot_action = action::OBSTRUCTED;
			new_pos.x = old_pos.x;
		}
		else {
			// Set new coordinates one step left
			patchbot_action = action::WALK;
			new_pos.x = old_pos.x - 1;
		}
		break;

	case(instruction_type::RIGHT):
		new_pos.y = old_pos.y;		

		if (old_pos.x + 1 >= parent->map.get_width()) {
			// When on the edge of the map
			patchbot_action = action::OBSTRUCTED;
			new_pos.x = old_pos.x;
		}
		else {
			// Set new coordinates one step right
			patchbot_action = action::WALK;
			new_pos.x = old_pos.x + 1;
		}
		break;

	case(instruction_type::WAIT):
		// Coordinates don't change
		new_pos.x = old_pos.x;
		new_pos.y = old_pos.y;
		patchbot_action = action::WAIT;
		break;

	default:
		break;
	}

	// Interact with tile
	if (patchbot_action == action::WALK 
		&& !is_robot_blocked(new_pos.x, new_pos.y, current_instruction)) {
		
		// See what happens if patchbit walks on the next tile
		switch (parent->map.get_tile(new_pos.x, new_pos.y)
			->interact(robot_type::PATCHBOT)) {
		case(action::DIE):
			loose_game();
			return;

		case(action::WIN):
			win_game();
			return;

		case(action::OBSTRUCTED):
			patchbot_action = action::OBSTRUCTED;
			break;

		case(action::WALK_AND_WAIT):
			/* WALK_AND_WAIT is for obstacels, where the robot makes one step
			an then has to wait one cycle (e.g. Alien Grass). For that append
			one wait to instruction queue*/
			p_controls->instruction_queue.insert(
				p_controls->instruction_queue.begin(),
				instruction(instruction_type::WAIT, 1));
			patchbot_action = action::WALK;
			break;

		case(action::OPEN_DOOR):
			// Add door to door loop for closing in the future
			add_open_door(new_pos.x, new_pos.y);
			patchbot_action = action::WAIT;
			break;

		case(action::WAIT):
			patchbot_action = action::WAIT;
			break;

		default:
			break;
		}
	}

	if (patchbot_action == action::WALK) {
		// Patchbot actually walk
		push_robot(new_pos.x, new_pos.y, current_instruction);
		parent->map.robots.move_robot(old_pos.x, old_pos.y, new_pos.x, new_pos.y);
	}
	else if (patchbot_action == action::OBSTRUCTED) {
		// End "step until obstructed" here
		if (p_controls->instruction_queue.size() > 0)
			if (p_controls->instruction_queue.front().amount == 0)
				p_controls->instruction_queue.erase(
					p_controls->instruction_queue.begin());
		move_patchbot();
	}
}

void game_logic::process_enemys()
{
	std::stack<std::vector<std::unique_ptr<state_machine>>::iterator> dead_robots;

	for (std::vector<std::unique_ptr<state_machine>>::iterator i = enemy_kis.begin();
		i != enemy_kis.end(); ++i) {
		unsigned int current_id = (*i)->get_id();
		(*i)->process();

		if (parent->map.robots.get_robots_location(current_id) == NULL) {
			dead_robots.push(i);
		}
	}

	while (!dead_robots.empty()) {
		enemy_kis.erase(dead_robots.top());
		dead_robots.pop();
	}
}

void game_logic::add_open_door(int x, int y)
{
	/* Push to open_door class */
	open_doors.push_front(open_door(x, y, time_steps));
}

void game_logic::update_doors()
{
	/* Pull opened doors, opened by enemies*/
	for (coords c : parent->map.robots.opened_doors) {
		add_open_door(c.x, c.y);
	}
	parent->map.robots.opened_doors.clear();

	while (!open_doors.empty()) {

		/* When there are open doors, check if it's time to close them */
		if (time_steps - open_doors.back().open_since >= duration) {

			/* When there are doors that are shedueld for closing, 
			check if tile is not occupied by a robot */
			if (parent->map.robots.get_robot(
				open_doors.back().x, open_doors.back().y)->type 
				== robot_type::NONE) {
				/* Close door again*/
				parent->map.get_tile(open_doors.back().x, open_doors.back().y)
					->interact(robot_type::NONE);

				open_doors.pop_back();
			}
			else {
				/* If a door that is to be closed is occupied by a robot,
				then add it to the blocked doors to check on next update
				if they can be closed*/
				blocked_doors.push_front(open_doors.back());
				open_doors.pop_back();
			}
		} else {
			/* The open_door with highest index is the open_door that go pushed
			first, hence if it is not ready to be closed none of the open_doors
			with lower index are ready to be closed*/
			break;
		}
	}

	for (open_door o : blocked_doors) {
		// Check if previously blocked doors are now ready to be closed
		if (parent->map.robots.get_robot(
			blocked_doors.back().x, blocked_doors.back().y)->type
			== robot_type::NONE) {
			/* If so: close door*/
			parent->map.get_tile(blocked_doors.back().x, blocked_doors.back().y)
				->interact(robot_type::NONE);

			blocked_doors.pop_back();
		}
		else {
			/* If it is still occupied, push it to the front again,
			therefor we have a cycle that we go through on each update */
			blocked_doors.push_front(blocked_doors.back());
			blocked_doors.pop_back();
		}
	}
}

bool game_logic::is_robot_blocked(int x, int y, instruction_type in_dir) const
{
	if (parent->map.robots.get_robot(x, y)->type == robot_type::NONE)
		return false;

	switch (in_dir) {
	case(instruction_type::UP):
		y--;
		break;

	case(instruction_type::RIGHT):
		x++;
		break;

	case(instruction_type::DOWN):
		y++;
		break;

	case(instruction_type::LEFT):
		x--;
		break;
	default:
		return true;
	}

	if (parent->map.robots.get_robot(x, y)->type
		!= robot_type::NONE) return true;

	action tile_to_check = parent->map.get_tile(x, y)
		->interact(parent->map.robots.get_robot(x, y)->type);

	if (tile_to_check == action::OBSTRUCTED || tile_to_check == action::DIG)
		return true;

	if (tile_to_check == action::OPEN_DOOR) {
		parent->map.get_tile(x, y)->interact(robot_type::NONE);
		return true;
	}

	return false;
}

void game_logic::push_robot(int x, int y, instruction_type in_dir) 
{
	robot_type type = parent->map.robots.get_robot(x, y)->type;

	if (type == robot_type::NONE) return;

	coords robots_new_coords = coords(x, y);
	switch (in_dir) {
	case(instruction_type::UP):
		robots_new_coords.y--;
		break;

	case(instruction_type::RIGHT):
		robots_new_coords.x++;
		break;

	case(instruction_type::DOWN):
		robots_new_coords.y++;
		break;

	case(instruction_type::LEFT):
		robots_new_coords.x--;
		break;

	default:
		return;
	}

	if (parent->map.robots.get_robot(robots_new_coords.x, robots_new_coords.y)->type
		!= robot_type::NONE) return;

	action robots_fate = parent->map.get_tile(robots_new_coords.x, robots_new_coords.y)
		->interact(type);

	parent->map.robots.move_robot(x, y, robots_new_coords.x, robots_new_coords.y);

	if (robots_fate == action::DIE)
		parent->map.robots.kill_robot(parent->map.robots.get_robot(x, y)->id);
}

void game_logic::win_game()
{
	// If game is won:
	// Stop QTimer
	if (automation_timer->isActive())
		automation_timer->stop();
	QMessageBox::about(parent, "Gewonnen", "Du hast die Menschen gerettet! "
		"Jetzt wird die Erde wieder an die Rohstoffe kommen!");

	// Reset enviroment
	reset();
}

void game_logic::loose_game()
{
	// If game is lost:
	// Stop QTimer
	if(automation_timer->isActive())
		automation_timer->stop();
	QMessageBox::about(parent, "Verloren", "Du hast leider verloren! "
		"Jetzt wird die Erde niemals wieder an die Rohstoffe kommen...");
	reset();
}

void game_logic::reset()
{
	p_rendering_engine->game_is_on = false;

	parent->reset();
	parent->deactivate_mission_control_btns();
	parent->activate_instruction_btns();
	
	p_controls->reset(true);
	
	p_rendering_engine->do_refresh_render();
	
	open_doors.clear();
	blocked_doors.clear();
}

void game_logic::start_game() 
{
	p_rendering_engine->game_is_on = true;

	p_controls->make_instruction_backup();
	p_controls->deactivate_instructions_contorls();
	p_controls->activate_mission_control_contorls();

	parent->map.init_map_graph_struct();
	dijkstra::run_path_finding(parent->map);
	initialize_enemy_kis();

	p_rendering_engine->do_refresh_render();
}

void game_logic::cancel_game()
{
	loose_game();
}


void game_logic::single_step()
{
	parent->map.reset_all_tile_nodes();

	// Patchbots turn
	move_patchbot();

	if (!p_rendering_engine->game_is_on) return;

	// Find shortest way to patchbot
	dijkstra::run_path_finding(parent->map);

	process_enemys();

	update_doors();
	time_steps++;

	p_rendering_engine->do_refresh_render();

	if (!parent->map.robots.has_pb) loose_game();
}

void game_logic::automatic_stepping()
{
	p_controls->deactivate_mission_control_contorls(true);
	automation_timer->start(1000);
}

void game_logic::stop_automatic_stepping()
{
	automation_timer->stop();
	p_controls->activate_mission_control_contorls(true);
}