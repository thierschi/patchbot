#include "pb_controls.h"

#include <QMessageBox>

controls::controls(main_window* _parent) :
	parent(_parent)
{
	connect_to_parent();
	parent->adjust_instruction_edit_scrollbar();
}

void controls::connect_to_parent()
{
	QObject::connect(parent, &main_window::new_instruction,
		this, &controls::on_new_instruction);
	QObject::connect(parent, &main_window::reset_instructions,
		this, &controls::reset);
}

void controls::make_instruction_backup()
{
	backup = instruction_queue;
}

void controls::activate_instructions_contorls()
{
	parent->activate_instruction_btns();
}

void controls::deactivate_instructions_contorls()
{
	parent->deactivate_instruction_btns();
}

void controls::activate_mission_control_contorls(bool auto_mode)
{
	parent->activate_mission_control_btns(auto_mode);
}

void controls::deactivate_mission_control_contorls(bool auto_mode)
{
	parent->deactivate_mission_control_btns(auto_mode);
}

void controls::on_new_instruction(instruction _instruction) 
{
	if (_instruction.type == instruction_type::WAIT
		&& _instruction.amount == 0) {
		QMessageBox::about(parent, "Fehler", "Option \"Bis Hindernis\" "
			"ist inkompatibel mit Instruktion \"Warten\"");
		return;
	}
	if (_instruction.type == instruction_type::DEL) {
		if (!instruction_queue.empty()) {
			parent->del_in_instruction_line_edit(QString::number(
				instruction_queue.back().amount).length());
			instruction_queue.pop_back();
		}
		return;
	}
	if (!instruction_queue.empty()) {
		if (instruction_queue.back().type == _instruction.type) {
			parent->del_in_instruction_line_edit(QString::number(
				instruction_queue.back().amount).length());
			instruction_queue.back().amount += _instruction.amount;
			parent->add_to_instruction_line_edit(instruction_queue.back());
			return;
		}
	}
	instruction_queue.push_back(_instruction);
	parent->add_to_instruction_line_edit(_instruction);
}

void controls::reset(bool keep_instructions)
{
	if (keep_instructions) {
		instruction_queue = backup;
		backup = std::vector<instruction>();

		for (instruction _instruction : instruction_queue)
			parent->add_to_instruction_line_edit(_instruction);
	}
	else {
		instruction_queue = std::vector<instruction>();
		backup = std::vector<instruction>();
	}
}
