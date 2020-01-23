#pragma once
#include <pb_main_window.h>

#include <QMainWindow>
#include <QScrollBar>

class rendering_window : public main_window {
private:
    bool initial_render_complete;
    int h_value_before_scroll;
    int v_value_before_scroll;
    int padding_top;
    int padding_left;

public:
    img_resources resources;
    tile_map map;
    QImage qimg;

    rendering_window(QWidget* parent = 0,
        const std::string& path_to_resources = ".");
    ~rendering_window();

    void resizeEvent(QResizeEvent* event);

    int get_full_width_px() const;
    int get_full_height_px() const;

    void render_pixel(int x = 0, int y = 0);
    void initial_render();

private slots:
    /* Slots for scrollbars */
    void on_map_h_scrollbar_valueChanged(int value);
    void on_map_v_scrollbar_valueChanged(int value);
};