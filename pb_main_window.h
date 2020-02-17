#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "pb_map.h"

#include <QMainWindow>
#include <QScrollBar>
#include <QScrollArea>

enum class instruction_type {
    UP = 'U',
    DOWN = 'D',
    LEFT = 'L',
    RIGHT = 'R',
    WAIT = 'W', 
    DEL
};

class instruction {
public:
    instruction_type type;
    /* If 0: Until obstruction */
    int amount;

    instruction(instruction_type type_, int amount_);
};

QT_BEGIN_NAMESPACE
namespace Ui { class main_window; }
QT_END_NAMESPACE

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    Ui::main_window* ui;
    tile_map map;

    main_window(QWidget* parent = 0);
    ~main_window();

    void resizeEvent(QResizeEvent* event);

    int get_map_scrollArea_width() const;
    int get_map_scrollArea_height() const;

    // Setter for rendering_engine
    void resize_map_placeholder(int width, int height);
    void map_placeholder_set_pixmap(const QPixmap& pixmap);
    void adjust_scrollbars(int l_value, int t_value);
    void refresh_colonie_label();
    void set_resource_size(int width, int height);

    // Setter for contorls
    void adjust_instruction_edit_scrollbar();
    void add_to_instruction_line_edit(const instruction& instruction_);
    void del_in_instruction_line_edit(int number_of_deletes);
    void activate_instruction_btns();
    void deactivate_instruction_btns();
    void activate_mission_control_btns(bool auto_mode = false);
    void deactivate_mission_control_btns(bool auto_mode = false);
    void reset();

public slots:
    /* Slots for buttons */
    void on_change_colonie_button_clicked();

    // Instructions
    void on_instruction_arrow_up_btn_clicked();
    void on_instruction_arrow_back_btn_clicked();
    void on_instruction_arrow_left_btn_clicked();
    void on_instruction_arrow_center_btn_clicked();
    void on_instruction_arrow_right_btn_clicked();
    void on_instruction_arrow_down_btn_clicked();

    void on_m_c_start_btn_clicked();
    void on_m_c_cancel_btn_clicked();
    void on_m_c_step_btn_clicked();
    void on_m_c_auto_btn_clicked();
    void on_m_c_stop_btn_clicked();

    /* Slots for scrollbars */
    void on_map_h_scrollbar_valueChanged(int value);
    void on_map_v_scrollbar_valueChanged(int value);
    void on_instruction_control_h_scroll_valueChanged(int value);

signals:
    // Signals for rendering_engine
    void do_initial_render();
    void window_resized(const QScrollArea& resizee);
    void v_scroll(int value);
    void h_scroll(int value);
    void do_refresh_render();

    // Signals for controls
    void new_instruction(const instruction& instruction_);
    void reset_instructions(bool kepp_instrucions);

    // Signals for game logic
    void start_game();
    void single_step();
    void automatic_stepping();
    void stop_automatic_stepping();
    void cancel_game();

protected:
    int resource_width;
    int resource_height;
    std::string map_path;
};
#endif // MAIN_WINDOW_H