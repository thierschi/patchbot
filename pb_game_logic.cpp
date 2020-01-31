#include "pb_game_logic.h"
#include "pb_map.h"

#include <chrono>
#include <thread>
#include <QMessageBox>

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

void game_logic::move_patchbot()
{
	// If no instructions left -> Lost
	if (p_controls->instruction_queue.size() < 1) {
		loose_game();
		return;
	}

	// Get next instruction and delete if neccesary
	instruction_type current_instruction = p_controls->
		instruction_queue.front().type;
	if (p_controls->instruction_queue.front().amount == 1)
		p_controls->instruction_queue.erase(
			p_controls->instruction_queue.begin());
	else if (p_controls->instruction_queue.front().amount > 1)
		p_controls->instruction_queue.front().amount--;

	// Creat old and new coords
	coords old_pos = parent->
		map.robots.get_robots_location(robot_type::PATCHBOT);
	coords new_pos = coords();

	action patchbot_action;

	// Get new coords if patchbot would walk
	switch (current_instruction)
	{
	case(instruction_type::UP):
		new_pos.x = old_pos.x;

		if (old_pos.y - 1 < 0) {
			patchbot_action = action::OBSTRUCTED;
			new_pos.y = old_pos.y;
		}
		else {
			patchbot_action = action::WALK;
			new_pos.y = old_pos.y - 1;
		}
		break;

	case(instruction_type::DOWN):
		new_pos.x = old_pos.x;

		if (old_pos.y + 1 >= parent->map.get_height()) {
			patchbot_action = action::OBSTRUCTED;
			new_pos.y = old_pos.y;
		}
		else {
			patchbot_action = action::WALK;
			new_pos.y = old_pos.y + 1;
		}
		break;

	case(instruction_type::LEFT):
		new_pos.y = old_pos.y;

		if (old_pos.x - 1 < 0) {
			patchbot_action = action::OBSTRUCTED;
			new_pos.x = old_pos.x;
		}
		else {
			patchbot_action = action::WALK;
			new_pos.x = old_pos.x - 1;
		}
		break;

	case(instruction_type::RIGHT):
		new_pos.y = old_pos.y;		

		if (old_pos.x + 1 >= parent->map.get_width()) {
			patchbot_action = action::OBSTRUCTED;
			new_pos.x = old_pos.x;
		}
		else {
			patchbot_action = action::WALK;
			new_pos.x = old_pos.x + 1;
		}
		break;

	case(instruction_type::WAIT):
		new_pos.x = old_pos.x;
		new_pos.y = old_pos.y;
		patchbot_action = action::WAIT;
		break;

	default:
		throw std::invalid_argument("Invalid argument in single_step: "
			"Invalid instruction passed.");
		break;
	}

	// Interact with tile
	if (patchbot_action == action::WALK) {
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
			p_controls->instruction_queue.insert(
				p_controls->instruction_queue.begin(),
				instruction(instruction_type::WAIT, 1));
			patchbot_action = action::WALK;
			break;
		case(action::OPEN_DOOR):
			// Add door to door loop for closing in the future
			open_doors.push_front(open_door(new_pos.x, new_pos.y, time_steps));
			patchbot_action = action::WAIT;
			break;
		case(action::WAIT):
			patchbot_action = action::WAIT;
			break;
		default:
			break;
		}
	}

	// Patchbit walk
	if (patchbot_action == action::WALK)
		parent->map.robots.move_robot(old_pos.x, old_pos.y,
			new_pos.x, new_pos.y);
	else if (patchbot_action == action::OBSTRUCTED)
		if (p_controls->instruction_queue.size() > 0)
			if (p_controls->instruction_queue.front().amount == 0)
				p_controls->instruction_queue.erase(
					p_controls->instruction_queue.begin());
}

void game_logic::update_doors()
{
	while (!open_doors.empty()) {
		if (time_steps - open_doors.back().open_since >= duration) {
			if (parent->map.robots.get_robot(
				open_doors.back().x, open_doors.back().y).type 
				== robot_type::NONE) {
				parent->map.get_tile(open_doors.back().x, open_doors.back().y)
					->interact(robot_type::NONE);
				open_doors.pop_back();
			}
			else {
				blocked_doors.push_front(open_doors.back());
				open_doors.pop_back();
			}
		} else {
			break;
		}
	}

	for (open_door o : blocked_doors) {
		if (parent->map.robots.get_robot(
			blocked_doors.back().x, blocked_doors.back().y).type
			== robot_type::NONE) {
			parent->map.get_tile(blocked_doors.back().x, blocked_doors.back().y)
				->interact(robot_type::NONE);
			blocked_doors.pop_back();
		}
		else {
			blocked_doors.push_front(blocked_doors.back());
			blocked_doors.pop_back();
		}
	}
}

void game_logic::win_game()
{
	if (automation_timer->isActive())
		automation_timer->stop();
	QMessageBox::about(parent, "Gewonnen", "Du hast die Menschen gerettet! "
		"Jetzt wird die Erde wieder an die Rohstoffe kommen!");
	reset();
}

void game_logic::loose_game()
{
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
	p_controls->make_instruction_backup();
	p_controls->deactivate_instructions_contorls();
	p_controls->activate_mission_control_contorls();
	p_rendering_engine->game_is_on = true;
	p_rendering_engine->do_refresh_render();
}

void game_logic::single_step()
{
	move_patchbot();

	/*
		TODO: Gegner sind am Zug
	*/

	update_doors();
	time_steps++;

	p_rendering_engine->do_refresh_render();
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

void game_logic::cancel_game()
{
	loose_game();
}
