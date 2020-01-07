#include "pb_main_window.h"
#include "ui_pb_main_window.h"

#include <QMessageBox>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::main_window)
{
    ui->setupUi(this);
}

main_window::~main_window()
{
    delete ui;
}


void main_window::on_change_colonie_button_clicked()
{
    QMessageBox::about(this, "Dummy", "Close your eyes and think about changing to a different colonie.");
}

void main_window::on_prg_arrow_up_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Close your eyes and think about how \"O\" and a number get appended to the textbox to the right.");
}

void main_window::on_prg_arrow_back_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Close your eyes and think about how the last character of the right textbox gets deleted.");
}

void main_window::on_prg_arrow_left_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Close your eyes and think about how \"L\" and a number get appended to the textbox to the right.");
}

void main_window::on_prg_arrow_center_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Close your eyes and think about how \"W\" and a number get appended to the textbox to the right.");
}

void main_window::on_prg_arrow_right_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Close your eyes and think about how \"R\" and a number get appended to the textbox to the right.");
}

void main_window::on_prg_arrow_down_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Close your eyes and think about how \"D\" and a number get appended to the textbox to the right.");
}

void main_window::on_m_c_start_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "This button gets me started. :3");
}

void main_window::on_m_c_cancel_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "This button gets me canceled. :C");
}

void main_window::on_m_c_step_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Let's make a step.");
}

void main_window::on_m_c_auto_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "Let's automate some things, shall we?");
}

void main_window::on_m_c_stop_btn_clicked()
{
    QMessageBox::about(this, "Dummy", "STOP! Wait a minute...");
}
