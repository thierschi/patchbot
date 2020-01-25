#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include "pb_img.h"

#include <QMainWindow>
#include <QScrollBar>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
namespace Ui { class main_window; }
QT_END_NAMESPACE

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    Ui::main_window* ui;
    tile_map map;

    main_window(QWidget* parent = 0,
        const std::string& path_to_resources = ".");
    ~main_window();

    void resizeEvent(QResizeEvent* event);

    int get_map_scrollArea_width() const;
    int get_map_scrollArea_height() const;

    void resize_map_placeholder(int width, int height);
    void map_placeholder_set_pixmap(const QPixmap& pixmap);
    void adjust_scrollbars();
    void refresh_colonie_label();
    void set_resource_size(int width, int height);

public slots:
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

signals:
    void do_initial_render();
    void window_resized(const QScrollArea& resizee);
    void v_scroll(int value);
    void h_scroll(int value);

protected:
    int resource_width;
    int resource_height;
};
#endif // MAIN_WINDOW_H