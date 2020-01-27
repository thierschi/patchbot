#pragma once
#include "pb_main_window.h"
#include "pb_rendering_engine.h"
#include "pb_controls.h"

#include <QObject>
#include <queue>
#include <QTimer>

class open_door {
public:
	int x;
	int y;
	int remaining_open_for;

	open_door(int _x, int _y, int _remaining_open_for = 10);
};

class game_logic : public QObject {

	Q_OBJECT

public:
	game_logic(main_window* _parent,
		rendering_engine* _rendering_engine, controls* _controls);
	game_logic(const game_logic&) = default;
	~game_logic();

	void connect_to_parent();

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
	main_window* parent;
	rendering_engine* __rendering_engine;
	controls* __controls;
	
	QTimer *automation_timer;
	std::queue<open_door> open_doors;
};

