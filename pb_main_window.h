#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include "pb_img.h"

#include <QMainWindow>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
namespace Ui { class main_window; }
QT_END_NAMESPACE

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    img_resources resources;
    tile_map map;
    QImage qimg;

    main_window(QWidget *parent = 0, 
        const std::string& path_to_resources = ".");
    ~main_window();
  
    void resizeEvent(QResizeEvent* event);

    /* Getter */
    int get_full_width_px() const;
    int get_full_height_px() const;

    void render_pixel(int x = 0, int y = 0);
    void adjust_scrollbars();
    void initial_render();

private slots:
    /* Slots for buttons */
    void on_change_colonie_button_clicked();
    void on_prg_arrow_up_btn_clicked();
    void on_prg_arrow_back_btn_clicked();
    void on_prg_arrow_left_btn_clicked();
    void on_prg_arrow_center_btn_clicked();
    void on_prg_arrow_right_btn_clicked();
    void on_prg_arrow_down_btn_clicked();
    void on_m_c_start_btn_clicked();
    void on_m_c_cancel_btn_clicked();
    void on_m_c_step_btn_clicked();
    void on_m_c_auto_btn_clicked();
    void on_m_c_stop_btn_clicked();

    /* Slots for scrollbars */
    void on_map_h_scrollbar_valueChanged(int value);
    void on_map_v_scrollbar_valueChanged(int value);

private:
    bool initial_render_complete;
    int h_value_before_scroll;
    int v_value_before_scroll;
    int padding_top;
    int padding_left;

    Ui::main_window *ui;
};
#endif // MAIN_WINDOW_H
