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
    resources(img_resources(path_to_resources)),
    map(tile_map()),
    qimg(QImage()),
    initial_render_complete(false),
    h_value_before_scroll(0),
    v_value_before_scroll(0),
    padding_top(0),
    padding_left(0)
{
    try {
        map = pb_input::read_map_txt(".\\txt\\hunted.txt");
    }
    catch (std::exception e) {
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
    /* ResizeEvent gets also called as the window shows up,
    so to prevent resizeEvent from messing up the initial_render,
    bool initial_render_completed exists*/
    if (initial_render_complete) {
        int old_width = qimg.width();
        int old_height = qimg.height();

        ui->map_placeholder->resize(
            /* If new width is out of range of the full map view,
            set width to the width of the visible part of the map,
            otherwise the tile_map will throw out of range errors*/
            (get_full_width_px() < ui->map_scrollArea->width() + padding_left)
            ? get_full_width_px() - padding_left
            : ui->map_scrollArea->width(),
            // The same goes for the height
            (get_full_height_px() < ui->map_scrollArea->height() + padding_top)
            ? get_full_height_px() - padding_top
            : ui->map_scrollArea->height());

        qimg = qimg.copy(0, 0,
            // Same as with the map_placeholder
            (get_full_width_px() < ui->map_scrollArea->width() + padding_left)
            ? get_full_width_px() - padding_left
            : ui->map_scrollArea->width(),
            qimg.height());


        for (int x = old_width; x < qimg.width(); x++) {
            for (int y = 0; y < old_height; y++) {
                // Render currently unrendered pixel
                render_pixel(x, y);
            }
        }

        qimg = qimg.copy(0, 0, qimg.width(),
            // Same as with the map_placeholder
            (get_full_height_px() < ui->map_scrollArea->height() + padding_top)
            ? get_full_height_px() - padding_top
            : ui->map_scrollArea->height());

        for (int y = old_height; y < qimg.height(); y++) {
            for (int x = 0; x < qimg.width(); x++) {
                // Render currently unrendered pixel
                render_pixel(x, y);
            }
        }

        // Display the fresh image
        ui->map_placeholder->setPixmap(QPixmap::fromImage(qimg));
        adjust_scrollbars();
    }
}


int main_window::get_full_width_px() const
{
    return map.get_width() 
        * resources.get_tga(map.get_tile(0, 0))->header.img_width;
}

int main_window::get_full_height_px() const
{
    return map.get_height() 
        * resources.get_tga(map.get_tile(0, 0))->header.img_height;
}

void main_window::render_pixel(int x, int y)
{
    /* Calc corresponding tile to coords*/
    int map_x = floor((x + padding_left) / resources.get_tga(map.get_tile(0, 0))
        ->header.img_width);
    int map_y = floor((y + padding_top) / resources.get_tga(map.get_tile(0, 0))
        ->header.img_height);
    /* Calc corresponding pixel of a tga tile image to coords */
    int pixel_x = (x + padding_left) % resources.get_tga(map.get_tile(0, 0))
        ->header.img_width;
    int pixel_y = (y + padding_top) % resources.get_tga(map.get_tile(0, 0))
        ->header.img_height;

    rgba_pixel pixel = resources.get_tga((
        map.get_tile(map_x, map_y)
        ))->get_pixel(pixel_x, resources.get_tga(map.get_tile(0, 0))
            ->header.img_height - 1 - pixel_y);

    /* Blend pixel of robot on top, if robot is present on field */
    if (map.robots.get_robot(map_x, map_y).type != robot_type::NONE)
        pixel.overlay_pixel(resources.get_tga(
            map.robots.get_robot(map_x, map_y)
        )->get_pixel(pixel_x, resources.get_tga(map.get_tile(0, 0))
            ->header.img_height - 1 - pixel_y));

    qimg.setPixel(x, y, qRgba(
        pixel.red,
        pixel.green,
        pixel.blue,
        pixel.alpha
    ));
}

void main_window::adjust_scrollbars()
{
    ui->map_h_scrollbar->setPageStep(ui->map_scrollArea->width());
    ui->map_v_scrollbar->setPageStep(ui->map_scrollArea->height());
    ui->map_v_scrollbar->setRange(0, get_full_height_px()
        - ui->map_v_scrollbar->pageStep());
    ui->map_h_scrollbar->setRange(0, get_full_width_px()
        - ui->map_h_scrollbar->pageStep());
}

void main_window::initial_render()
{
    ui->map_placeholder->resize(
        /* If width is out of range of the full map view,
            set width to the width of the visible part of the map,
            otherwise the tile_map will throw out of range errors*/
        (ui->map_scrollArea->width() < get_full_width_px()) ?
        ui->map_scrollArea->width() : get_full_width_px(),
        // same goes here
        (ui->map_scrollArea->height() < get_full_height_px()) ?
        ui->map_scrollArea->height() : get_full_height_px());

    // Init qimg
    qimg = QImage(ui->map_placeholder->width(), ui->map_placeholder->height(),
        QImage::Format_RGBA8888);

    for (int y = 0; y < qimg.height(); y++) {
        for (int x = 0; x < qimg.width(); x++) {
            // Render whole visible part of map
            render_pixel(x, y);
        }
    }

    ui->map_placeholder->setPixmap(QPixmap::fromImage(qimg));
    adjust_scrollbars();

    // Set the colonie name
    QString name = "Aktuelle Kolonie: ";
    name.append(map.name.c_str());
    ui->colonie_label->setText(name);

    /* Signal that initial rendering is complete so that resizing
    and scrolling works */
    initial_render_complete = true;
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
    catch (std::exception e) {
        QMessageBox::about(this, "Fehler", e.what());
        return;
    }
    // Reset image
    qimg = QImage();

    // Reset all values and reset scrollbar
    initial_render_complete = false;
    h_value_before_scroll = 0;
    v_value_before_scroll = 0;
    ui->map_h_scrollbar->setValue(0);
    ui->map_v_scrollbar->setValue(0);
    padding_top = 0;
    padding_left = 0;

    /* Render new map */
    initial_render();
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
    if (initial_render_complete) {
        // Calculate distance scrolled
        int d_value = value - h_value_before_scroll;
        h_value_before_scroll = value;

        // Move qimg and save by how much the map is moved 
        qimg = qimg.copy(d_value, 0, qimg.width(), qimg.height());
        padding_left = value;

        // Check in which direction one scrolled
        if (d_value > 0) {        
            for (int x = qimg.width() - d_value; x < qimg.width(); x++) {
                for (int y = 0; y < qimg.height(); y++) {
                    // Render now blank pixels
                    render_pixel(x, y);
                }
            }
        }
        else {
            for (int x = 0; x < 0 - d_value; x++) {
                for (int y = 0; y < qimg.height(); y++) {
                    // Render now blank pixels
                    render_pixel(x, y);
                }
            }
        }

        ui->map_placeholder->setPixmap(QPixmap::fromImage(qimg));
        return;
    }
    // Reset Scrollbar if one tries to scroll before the map was rendered
    ui->map_h_scrollbar->setValue(0);
}

void main_window::on_map_v_scrollbar_valueChanged(int value)
{
    if (initial_render_complete) {
        // Calculate distance scrolled
        int d_value = value - v_value_before_scroll;
        v_value_before_scroll = value;

        // Move qimg and save by how much the map is moved 
        qimg = qimg.copy(0, d_value, qimg.width(), qimg.height());
        padding_top = value;

        // Check in which direction one scrolled
        if (d_value > 0) {
            for (int y = qimg.height() - d_value; y < qimg.height(); y++) {
                for (int x = 0; x < qimg.width(); x++) {
                    // Render now blank pixels
                    render_pixel(x, y);
                }
            }
        }
        else {
            for (int y = 0; y < 0 - d_value; y++) {
                for (int x = 0; x < qimg.width(); x++) {
                    // Render now blank pixels
                    render_pixel(x, y);
                }
            }
        }

        ui->map_placeholder->setPixmap(QPixmap::fromImage(qimg));
        return;
    }
    // Reset Scrollbar if one tries to scroll before the map was rendered
    ui->map_v_scrollbar->setValue(0);
}
