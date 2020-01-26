#include "pb_game_logic.h"

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
	__controls(_controls)
{
	connect_to_parent();
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

void game_logic::lost_game()
{
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
	__controls->deactivate_instructions_contorls();
	__controls->activate_mission_control_contorls();
	__rendering_engine->game_is_on = true;
	__rendering_engine->refresh_render();
}

void game_logic::single_step()
{
	QMessageBox::about(parent, "This...", "...is a test");
}

void game_logic::automatic_stepping()
{
	__controls->deactivate_mission_control_contorls(true);
	
	automation_timer = new QTimer(parent);
	QObject::connect(automation_timer, &QTimer::timeout,
		this, &game_logic::single_step);
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

