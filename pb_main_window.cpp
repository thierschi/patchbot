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

main_window::main_window(QWidget* parent,
    const std::string& path_to_resources)
    : QMainWindow(parent),
    ui(new Ui::main_window),
    map(tile_map())
{
    try {
        map = pb_input::read_map_txt(".\\txt\\hunted.txt");
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
    ui->map_v_scrollbar->setRange(0, resource_width
        - ui->map_v_scrollbar->pageStep());
    ui->map_h_scrollbar->setRange(0, resource_height
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

void main_window::on_change_colonie_button_clicked()
{
    // Open file Dialoge
    QString file_name = QFileDialog::getOpenFileName(this,
        tr("Karten-Textdatei öffnen..."), ".\\txt", tr("Text Files (*.txt)"));

    // Load new map
    try {
        map = pb_input::read_map_txt(file_name.toLocal8Bit().constData());
    }
    catch (std::exception &e) {
        QMessageBox::about(this, "Fehler", e.what());
        return;
    }

    // Reset all values and reset scrollbar
    ui->map_h_scrollbar->setValue(0);

    /* Render new map */
    emit do_initial_render();
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

void main_window::on_map_h_scrollbar_valueChanged(int value)
{
    emit h_scroll(value);
}

void main_window::on_map_v_scrollbar_valueChanged(int value)
{
    emit v_scroll(value);
}
