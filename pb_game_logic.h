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

	open_door(int x_, int y_, int open_since_);
};

class game_logic : public QObject {

	Q_OBJECT

public:
	game_logic(main_window* parent_,
		rendering_engine* rendering_engine_, controls* controls_);
	game_logic(const game_logic&) = default;
	~game_logic();

	void connect_to_parent();

	void move_patchbot();
	void update_doors();

	void win_game();
	void loose_game();
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
	rendering_engine* p_rendering_engine;
	controls* p_controls;
	
	QTimer *automation_timer;
	std::list<open_door> open_doors;
	std::list<open_door> blocked_doors;
};

