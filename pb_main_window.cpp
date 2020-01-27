#include "pb_main_window.h"
#include "ui_pb_main_window.h"
#include "pb_img.h"
#include "pb_io.h"
#include "pb_map.h"
#include "pb_exceptions.h"

#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <QResizeEvent>
#include <Windows.h>
#include <debugapi.h>

main_window::main_window(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::main_window),
    map(tile_map()),
    map_path(".\\txt\\hunted.txt")
{
    try {
        map = pb_input::read_map_txt(map_path);
    }
    catch (std::exception& e) {
        QMessageBox::about(this, "Fehler", e.what());
    }
    ui->setupUi(this);
}

main_window::~main_window()
{
    delete ui;
}

void main_window::resizeEvent(QResizeEvent* event)
{
    emit window_resized(*ui->map_scrollArea);
}

int main_window::get_map_scrollArea_width() const
{
    return ui->map_scrollArea->width();
}

int main_window::get_map_scrollArea_height() const
{
    return ui->map_scrollArea->height();
}

void main_window::resize_map_placeholder(int width, int height)
{
    ui->map_placeholder->resize(width, height);
}

void main_window::map_placeholder_set_pixmap(const QPixmap& pixmap)
{
    ui->map_placeholder->setPixmap(pixmap);
}

void main_window::adjust_scrollbars()
{
    ui->map_h_scrollbar->setPageStep(ui->map_scrollArea->width());
    ui->map_v_scrollbar->setPageStep(ui->map_scrollArea->height());
    ui->map_v_scrollbar->setRange(0, resource_height
        - ui->map_v_scrollbar->pageStep());
    ui->map_h_scrollbar->setRange(0, resource_width
        - ui->map_h_scrollbar->pageStep());
}

void main_window::refresh_colonie_label()
{
    QString name = "Aktuelle Kolonie: ";
    name.append(map.name.c_str());
    ui->colonie_label->setText(name);
}

void main_window::set_resource_size(int width, int height)
{
    resource_width = width;
    resource_height = height;
}

void main_window::adjust_instruction_edit_scrollbar()
{
    int temp = ui->instruction_edit->cursorPosition();
    ui->instruction_edit->end(false);
    ui->instruction_control_h_scroll->setPageStep(12);
    ui->instruction_control_h_scroll->setRange(0,
        ui->instruction_edit->cursorPosition()
        - ui->instruction_control_h_scroll->pageStep());
    ui->instruction_edit->setCursorPosition(temp);
    ui->instruction_control_h_scroll->setValue(temp);
}

void main_window::add_to_instruction_line_edit(const instruction& _instruction)
{
    QString qstr = (char)_instruction.type;
    qstr.append((_instruction.amount != 0) 
        ? QString::number(_instruction.amount) : (QString)'X');

    ui->instruction_edit->end(false);
    ui->instruction_edit->insert(qstr);

    adjust_instruction_edit_scrollbar();
}

void main_window::del_in_instruction_line_edit(int number_of_deletes)
{
    ui->instruction_edit->end(false);
    for (int i = 0; i < number_of_deletes + 1; i++) {
        ui->instruction_edit->backspace();
    }

    adjust_instruction_edit_scrollbar();
}

void main_window::activate_instruction_btns()
{
    ui->instruction_arrow_up_btn->setEnabled(true);
    ui->instruction_arrow_down_btn->setEnabled(true);
    ui->instruction_arrow_left_btn->setEnabled(true);
    ui->instruction_arrow_right_btn->setEnabled(true);
    ui->instruction_arrow_back_btn->setEnabled(true);
    ui->instruction_arrow_center_btn->setEnabled(true);
    ui->instruction_control_dropdown->setEnabled(true);
    ui->change_colonie_button->setEnabled(true);
}

void main_window::deactivate_instruction_btns()
{
    ui->instruction_arrow_up_btn->setEnabled(false);
    ui->instruction_arrow_down_btn->setEnabled(false);
    ui->instruction_arrow_left_btn->setEnabled(false);
    ui->instruction_arrow_right_btn->setEnabled(false);
    ui->instruction_arrow_back_btn->setEnabled(false);
    ui->instruction_arrow_center_btn->setEnabled(false);
    ui->instruction_control_dropdown->setEnabled(false);
    ui->change_colonie_button->setEnabled(false);
}

void main_window::activate_mission_control_btns(bool auto_mode)
{
    ui->m_c_step_btn->setEnabled(true);
    ui->m_c_auto_btn->setEnabled(true);
    ui->m_c_stop_btn->setEnabled(false);
    if (!auto_mode) {
        ui->m_c_start_btn->setEnabled(false);
        ui->m_c_cancel_btn->setEnabled(true);
    }
}

void main_window::deactivate_mission_control_btns(bool auto_mode)
{
    ui->m_c_step_btn->setEnabled(false);
    ui->m_c_auto_btn->setEnabled(false);
    ui->m_c_stop_btn->setEnabled(true);
    if (!auto_mode) {
        ui->m_c_start_btn->setEnabled(true);
        ui->m_c_stop_btn->setEnabled(false);
        ui->m_c_cancel_btn->setEnabled(false);
    }
}

void main_window::reset()
{
    // Load map again
    try {
        map = pb_input::read_map_txt(map_path);
    }
    catch (std::exception & e) {
        QMessageBox::about(this, "Fehler", e.what());
        return;
    }

    //Reset instruction_edit
    ui->instruction_edit->clear();
    ui->instruction_control_dropdown->setCurrentIndex(0);

    // Reset all values and reset scrollbar
    ui->map_h_scrollbar->setValue(0);
    ui->map_v_scrollbar->setValue(0);

    /* Render map again*/
    emit do_initial_render();
}

void main_window::on_change_colonie_button_clicked()
{
    // Open file Dialoge
    QString file_name = QFileDialog::getOpenFileName(this,
        tr("Karten-Textdatei öffnen..."), ".\\txt", tr("Text Files (*.txt)"));
    map_path = file_name.toStdString();

    reset();

    // Reset instructions
    emit reset_instructions(false);
}

void main_window::on_instruction_arrow_up_btn_clicked()
{
    emit new_instruction(instruction(instruction_type::UP,
        ui->instruction_control_dropdown->currentIndex()));
}

void main_window::on_instruction_arrow_back_btn_clicked()
{
    emit new_instruction(instruction(instruction_type::DEL, -1));
}

void main_window::on_instruction_arrow_left_btn_clicked()
{
    emit new_instruction(instruction(instruction_type::LEFT,
        ui->instruction_control_dropdown->currentIndex()));
}

void main_window::on_instruction_arrow_center_btn_clicked()
{
    emit new_instruction(instruction(instruction_type::WAIT,
        ui->instruction_control_dropdown->currentIndex()));
}

void main_window::on_instruction_arrow_right_btn_clicked()
{
    emit new_instruction(instruction(instruction_type::RIGHT,
        ui->instruction_control_dropdown->currentIndex()));
}

void main_window::on_instruction_arrow_down_btn_clicked()
{
    emit new_instruction(instruction(instruction_type::DOWN,
        ui->instruction_control_dropdown->currentIndex()));
}

void main_window::on_m_c_start_btn_clicked()
{
    emit start_game();
}

void main_window::on_m_c_cancel_btn_clicked()
{
    emit cancel_game();
}

void main_window::on_m_c_step_btn_clicked()
{
    emit single_step();
}

void main_window::on_m_c_auto_btn_clicked()
{
    emit automatic_stepping();
}

void main_window::on_m_c_stop_btn_clicked()
{
    emit stop_automatic_stepping();
}

void main_window::on_map_h_scrollbar_valueChanged(int value)
{
    emit h_scroll(value);
}

void main_window::on_map_v_scrollbar_valueChanged(int value)
{
    emit v_scroll(value);
}

void main_window::on_instruction_control_h_scroll_valueChanged(int value)
{
    ui->instruction_edit->setCursorPosition(value * 2);
}

instruction::instruction(instruction_type _type, int _amount) :
    type(_type), 
    amount(_amount)
{
}
