#pragma once
#include "pb_main_window.h"
#include "pb_rendering_engine.h"
#include "pb_controls.h"

#include <QObject>
#include <QTimer>
#include <list>

class open_door {
public:
	int open_since;
	int x;
	int y;

	open_door(int _x, int _y, int _open_since);
};

class game_logic : public QObject {

	Q_OBJECT

public:
	game_logic(main_window* _parent,
		rendering_engine* _rendering_engine, controls* _controls);
	game_logic(const game_logic&) = default;
	~game_logic();

	void connect_to_parent();

	void patchbots_turn();
	void doors();

	void won_game();
	void lost_game();
	void reset();

public slots:
	void start_game();
	void single_step();
	void automatic_stepping();
	void stop_automatic_stepping();
	void cancel_game();

private:
	int time_steps;
	int duration;

	main_window* parent;
	rendering_engine* __rendering_engine;
	controls* __controls;
	
	QTimer *automation_timer;
	std::list<open_door> open_doors;
	std::list<open_door> blocked_doors;
};

