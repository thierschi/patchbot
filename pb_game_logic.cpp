#include "pb_game_logic.h"
#include "pb_map.h"

#include <chrono>
#include <thread>
#include <QMessageBox>

open_door::open_door(int _x, int _y, int _remaining_open_for) :
	x(_x),
	y(_y),
	remaining_open_for(_remaining_open_for)
{
}

game_logic::game_logic(main_window* _parent, 
	rendering_engine* _rendering_engine, controls* _controls) :
	parent(_parent),
	__rendering_engine(_rendering_engine),
	__controls(_controls),
	automation_timer(new QTimer(this))
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

void game_logic::won_game()
{
	if (automation_timer->isActive())
		automation_timer->stop();
	QMessageBox::about(parent, "Gewonnen", "Du hast die Menschen gerettet! "
		"Jetzt wird die Erde wieder an die Rohstoffe kommen!");
	reset();
}

void game_logic::lost_game()
{
	if(automation_timer->isActive())
		automation_timer->stop();
	QMessageBox::about(parent, "Verloren", "Du hast leider verloren! "
		"Jetzt wird die Erde niemals wieder an die Rohstoffe kommen...");
	reset();
}

void game_logic::reset()
{
	__rendering_engine->game_is_on = false;
	parent->reset();
	parent->deactivate_mission_control_btns();
	parent->activate_instruction_btns();
	__controls->reset(true);
	open_doors = std::queue<open_door>();
}

void game_logic::start_game() 
{
	__controls->make_instruction_backup();
	__controls->deactivate_instructions_contorls();
	__controls->activate_mission_control_contorls();
	__rendering_engine->game_is_on = true;
	__rendering_engine->refresh_render();
}

void game_logic::single_step()
{
	if (__controls->instruction_queue.size() < 1) {
		lost_game();
		return;
	}

	instruction_type current_instruction = __controls->
		instruction_queue.front().type;
	if (__controls->instruction_queue.front().amount == 1)
		__controls->instruction_queue.erase(
			__controls->instruction_queue.begin());
	else if (__controls->instruction_queue.front().amount > 1)
		__controls->instruction_queue.front().amount--;

	coords old_pos = parent->
		map.robots.get_robots_location(robot_type::PATCHBOT);
	coords new_pos = coords();

	action patchbot_action;
	
	switch (current_instruction)
	{
	case(instruction_type::UP):
		new_pos.x = old_pos.x;		
		if (new_pos.y < 0) {
			patchbot_action = action::WAIT;
			new_pos.y = old_pos.y;
		}
		else {
			patchbot_action = action::WALK;
			new_pos.y = old_pos.y - 1;
		}
		break;
	case(instruction_type::DOWN):
		new_pos.x = old_pos.x;
		if (new_pos.y >= parent->map.get_height()) {
			patchbot_action = action::WAIT;
			new_pos.y = old_pos.y;
		}
		else {
			patchbot_action = action::WALK;
			new_pos.y = old_pos.y + 1;
		}
		break;
	case(instruction_type::LEFT):
		new_pos.y = old_pos.y;
		if (new_pos.x < 0) {
			patchbot_action = action::WAIT;
			new_pos.x = old_pos.x;
		}
		else {
			patchbot_action = action::WALK;
			new_pos.x = old_pos.x - 1;
		}
		break;
	case(instruction_type::RIGHT):
		new_pos.y = old_pos.y;
		if (new_pos.x >= parent->map.get_width()) {
			patchbot_action = action::WAIT;
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
	
	if (patchbot_action == action::WALK) {
		switch (parent->map.get_tile(new_pos.x, new_pos.y)
			->interact(robot_type::PATCHBOT)) {
		case(action::DIE):
			lost_game();
			return;
		case(action::WIN):
			won_game();
			return;
		case(action::OBSTRUCTED):
			if(__controls->instruction_queue.size() > 0)
				if (__controls->instruction_queue.front().amount == 0)
					__controls->instruction_queue.erase(
						__controls->instruction_queue.begin());
			patchbot_action = action::WAIT;
			break;
		case(action::WALK_AND_WAIT):
			__controls->instruction_queue.insert(
				__controls->instruction_queue.begin(),
				instruction(instruction_type::WAIT, 1));
			patchbot_action = action::WALK;
			break;
		case(action::OPEN_DOOR):
			open_doors.push(open_door(new_pos.x, new_pos.y));
			patchbot_action = action::WAIT;
			break;
		case(action::WAIT):
			patchbot_action = action::WAIT;
			break;
		default:
			break;
		}
	}

	if(patchbot_action == action::WALK)
		parent->map.robots.move_robot(old_pos.x, old_pos.y, 
			new_pos.x, new_pos.y);

	__rendering_engine->refresh_render();
}

void game_logic::automatic_stepping()
{
	__controls->deactivate_mission_control_contorls(true);
	automation_timer->start(1000);
}

void game_logic::stop_automatic_stepping()
{
	automation_timer->stop();

	__controls->activate_mission_control_contorls(true);
}

void game_logic::cancel_game()
{
	lost_game();
}

