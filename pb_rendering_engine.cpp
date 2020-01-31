#include "pb_rendering_engine.h"
#include "pb_main_window.h"

#include <QMainWindow>

rendering_engine::rendering_engine(main_window* parent_,
    const std::string& path_to_resources) :
    resources(img_resources(path_to_resources)),
    qimg(QImage()),
    initial_render_complete(false),
    v_value_before_scroll(0),
    h_value_before_scroll(0),
    scrollarea_width_before_scroll(0),
    scrollarea_height_before_scroll(0),
    padding_top(0),
    padding_left(0),
    parent(parent_),
    game_is_on(false)
{
    connect_to_parent();
}

int rendering_engine::get_full_width_px()
{
    return parent->map.get_width()
        * resources.get_terrain_img.at(parent->map.get_tile(0, 0)->get_terrain()).header.img_width;
}

int rendering_engine::get_full_height_px()
{
    return parent->map.get_height()
        * resources.get_terrain_img.at(parent->map.get_tile(0, 0)->get_terrain()).header.img_height;
}

void rendering_engine::render_pixel(int x, int y)
{
    /* Calc corresponding tile to coords*/
    int map_x = floor((x + padding_left) / resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).header.img_width);
    int map_y = floor((y + padding_top) / resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).header.img_height);
    /* Calc corresponding pixel of a tga tile image to coords */
    int pixel_x = (x + padding_left) % resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).header.img_width;
    int pixel_y = (y + padding_top) % resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).header.img_height;


    auto tile_type = parent->map.get_tile(map_x, map_y)->get_terrain();
    rgba_pixel pixel = resources.get_terrain_img.at((
        tile_type
        )).get_pixel(pixel_x, resources.get_terrain_img.at(
            parent->map.get_tile(0, 0)->get_terrain()).header.img_height - 1 - pixel_y);

    if (game_is_on) {
        /* Blend pixel of robot on top, if robot is present on field */
        if (parent->map.robots.get_robot(map_x, map_y).type != robot_type::NONE)
            pixel.overlay_pixel(resources.get_robot_img.at(
                parent->map.robots.get_robot(map_x, map_y).type)
                .get_pixel(pixel_x, resources.get_terrain_img.at(
                    parent->map.get_tile(0, 0)->get_terrain())
                    .header.img_height - 1 - pixel_y));

        if (parent->map.robots.is_grave(map_x, map_y))
            pixel.underlay_pixel(resources.get_robot_img.at(robot_type::DEAD)
                .get_pixel(pixel_x, resources.get_terrain_img.at(
                    parent->map.get_tile(0, 0)->get_terrain())
                    .header.img_height - 1 - pixel_y));
    }

    qimg.setPixel(x, y, qRgba(
        pixel.red,
        pixel.green,
        pixel.blue,
        pixel.alpha
    ));
}

void rendering_engine::connect_to_parent()
{
    /* Connect the parents singals to this' slot */
    QObject::connect(parent, &main_window::do_initial_render,
        this, &rendering_engine::do_initial_render);
    QObject::connect(parent, &main_window::window_resized,
        this, &rendering_engine::on_window_resized);
    QObject::connect(parent, &main_window::v_scroll,
        this, &rendering_engine::on_vertical_scroll);
    QObject::connect(parent, &main_window::h_scroll,
        this, &rendering_engine::on_horizontal_scroll);
    QObject::connect(parent, &main_window::do_refresh_render,
        this, &rendering_engine::do_refresh_render);
}

void rendering_engine::do_initial_render()
{
    parent->set_resource_size(get_full_width_px(), get_full_height_px());
    initial_render_complete = false;
    h_value_before_scroll = 0;
    v_value_before_scroll = 0;
    padding_top = 0;
    padding_left = 0;
    int width, height;
    
    if(parent->get_map_scrollArea_width() < get_full_width_px())
        width = parent->get_map_scrollArea_width();
    else
        width = get_full_width_px();

    if (parent->get_map_scrollArea_height() < get_full_height_px())
        height = parent->get_map_scrollArea_height();
    else
        height = get_full_height_px();
    parent->resize_map_placeholder(width, height);

    // Init qimg
    qimg = QImage(width, height, QImage::Format_RGBA8888);

    for (int y = 0; y < qimg.height(); y++) {
        for (int x = 0; x < qimg.width(); x++) {
            // Render whole visible part of map
            render_pixel(x, y);
        }
    }

    parent->map_placeholder_set_pixmap(QPixmap::fromImage(qimg));
    /*ui->map_placeholder->setPixmap(QPixmap::fromImage(qimg));*/
    parent->adjust_scrollbars();

    // Set the colonie name
    parent->refresh_colonie_label();

    /* Signal that initial rendering is complete so that resizing
    and scrolling works */
    initial_render_complete = true;
}

void rendering_engine::do_refresh_render()
{
    if (initial_render_complete) {
        for (int y = 0; y < qimg.height(); y++) {
            for (int x = 0; x < qimg.width(); x++) {
                // Render whole visible part of map
                render_pixel(x, y);
            }
        }

        parent->map_placeholder_set_pixmap(QPixmap::fromImage(qimg));
    }
}

void rendering_engine::on_window_resized(const QScrollArea& resizee)
{
    /* ResizeEvent gets also called as the window shows up,
    so to prevent resizeEvent from messing up the initial_render,
    bool initial_render_completed exists*/
    if (initial_render_complete) {
        int old_width = qimg.width();
        int old_height = qimg.height();

        int width, height;

        padding_top = std::max(0, padding_top - (resizee.height() - scrollarea_height_before_scroll));
        padding_left = std::max(0, padding_left - (resizee.width() - scrollarea_width_before_scroll));

        if (get_full_width_px() < resizee.width() + padding_left) {
            width = get_full_width_px() - padding_left;
        }
        else
            width = parent->get_map_scrollArea_width();
        if (get_full_height_px() < resizee.height() + padding_top) {
            
            height = get_full_height_px() - padding_top;    
        }
        else
            height = parent->get_map_scrollArea_height();
        parent->resize_map_placeholder(width, height);

        qimg = qimg.copy(0, 0, width, qimg.height());
        qimg = qimg.copy(0, 0, qimg.width(), height);

        for (int x = old_width; x < qimg.width(); x++) {
            for (int y = 0; y < old_height; y++) {
                // render currently unrendered pixel
                render_pixel(x, y);
            }
        }

        for (int y = old_height; y < qimg.height(); y++) {
            for (int x = 0; x < qimg.width(); x++) {
                // render currently unrendered pixel
                render_pixel(x, y);
            }
        }

        scrollarea_width_before_scroll = resizee.width();
        scrollarea_height_before_scroll = resizee.height();

        // Display the fresh image
        parent->map_placeholder_set_pixmap(QPixmap::fromImage(qimg));
        parent->adjust_scrollbars();
    }
}

void rendering_engine::on_vertical_scroll(int value)
{
    if (initial_render_complete) {
        // Calculate distance scrolled
        int d_value = value - v_value_before_scroll;

        if (abs(d_value) >= qimg.height()) {
            padding_top = value;
            do_refresh_render();
        } 
        else {
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

            parent->map_placeholder_set_pixmap(QPixmap::fromImage(qimg));
        }
        return;
    }
}

void rendering_engine::on_horizontal_scroll(int value)
{
    if (initial_render_complete) {
        // Calculate distance scrolled
        int d_value = value - h_value_before_scroll;

        if (abs(d_value) >= qimg.width()) {
            padding_left = value;
            do_refresh_render();
        }
        else {
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

            parent->map_placeholder_set_pixmap(QPixmap::fromImage(qimg));
        }
        return;
    }
}