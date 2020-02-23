#pragma once
#include "pb_main_window.h"
#include "pb_rendering_engine.h"
#include "pb_controls.h"
#include "pb_state_machine.h"

#include <QObject>
#include <QTimer>
#include <list>
#include <vector>
#include <memory>

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

	/* Connect all slots of this to all signals of parent */
	void connect_to_parent();
	void initialize_enemy_kis();

	void move_patchbot();
	void process_enemys();

	void add_open_door(int x, int y);
	void update_doors();
	bool is_robot_blocked(int x, int y, instruction_type in_dir) const;
	void push_robot(int x, int y, instruction_type in_dir);

	void win_game();
	void loose_game();
	
	void reset();

public slots:
	void start_game();
	void cancel_game();

	void single_step();
	void automatic_stepping();
	void stop_automatic_stepping();

private:
	int time_steps;
	int duration;

	main_window* parent;
	rendering_engine* p_rendering_engine;
	controls* p_controls;
	
	QTimer *automation_timer;
	std::list<open_door> open_doors;
	std::list<open_door> blocked_doors;
	std::vector<std::unique_ptr<state_machine>> enemy_kis;
};

