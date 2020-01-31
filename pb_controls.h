#pragma once
#include "pb_main_window.h"

#include <QWidget>
#include <vector>

class controls : public QWidget{

	Q_OBJECT
	
public:
	std::vector<instruction> instruction_queue;

	controls(main_window* parent_);
	controls(const controls&) = default;

	void connect_to_parent();
	void make_instruction_backup();

	void activate_instructions_contorls();
	void deactivate_instructions_contorls();
	void activate_mission_control_contorls(bool auto_mode = false);
	void deactivate_mission_control_contorls(bool auto_mode = false);

public slots:
	void on_new_instruction(const instruction& instruction_);
	void reset(bool keep_instructions = false);

private:
	main_window* parent;
	std::vector<instruction> backup;
};

