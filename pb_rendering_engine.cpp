#include "pb_rendering_engine.h"
#include "pb_main_window.h"

#include <QMainWindow>
#include <QPainter>

rendering_engine::rendering_engine(main_window* parent_,
    const std::string& path_to_resources) :
    resources(img_resources(path_to_resources)),
    pixmap(QPixmap()),
    is_path_debug_mode(true),
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
        * resources.get_terrain_img.at(parent->map.get_tile(0, 0)
            ->get_terrain()).width();
}

int rendering_engine::get_full_height_px()
{
    return parent->map.get_height()
        * resources.get_terrain_img.at(parent->map.get_tile(0, 0)
            ->get_terrain()).height();
}

void rendering_engine::draw_tile(int x, int y)
{
    /* Calc corresponding tile to coords*/
    int map_x = floor((x + padding_left) / resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).width());
    int map_y = floor((y + padding_top) / resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).height());
    
    /* Get tile_terrain of tile at ( map_x | map_y ) */
    auto tile_type = parent->map.get_tile(map_x, map_y)->get_terrain();

    /* Draw background (map) */
    painter.drawPixmap(x, y, resources.get_terrain_img.at(tile_type));

    /* Only render robots and death_marks if game is actually "on" */
    if (game_is_on) {
        /* Draw death mark, if robot died on this tile */
        if (parent->map.robots.is_grave(map_x, map_y))
            painter.drawPixmap(x, y, resources.get_robot_img.at(robot_type::DEAD));

        /* Draw robot on top of background if present on this field */
        if (parent->map.robots.get_robot(map_x, map_y)->type != robot_type::NONE)
            painter.drawPixmap(x, y, resources.get_robot_img.at(
                parent->map.robots.get_robot(map_x, map_y)->type));

        /* If we want to see the arrows for checking that the implemented
        dijkstra algortihmn is working, draw those on top */
        if (is_path_debug_mode) {
            if (parent->map.get_tile(map_x, map_y)
                ->predecessor <= direction::WEST) {
                painter.drawPixmap(x, y, resources.get_arrow_img.at(
                    parent->map.get_tile(map_x, map_y)->predecessor));
            }
        }
    }
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
    /* Initialize variables */
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

    pixmap = QPixmap(width, height);

    /* Get width and height of tile graphics */
    int img_width = resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).width();
    int img_height = resources.get_terrain_img.at(
        parent->map.get_tile(0, 0)->get_terrain()).height();

    painter.begin(&pixmap);
    for (int y = 0; y <= pixmap.height() / img_height 
        && y < parent->map.get_height(); y++) {
        for (int x = 0; x <= pixmap.width() / img_width
            && x < parent->map.get_width(); x++) {
            /* Draw tile at x, y where (x|y) % 32 = 0 */
            draw_tile(x * img_width, y * img_height);
        }
    }
    painter.end();

    parent->map_placeholder_set_pixmap(pixmap);

    // Set the colonie name
    parent->refresh_colonie_label();
    
    parent->adjust_scrollbars(0,0);

    /* Signal that initial rendering is complete so that resizing
    and scrolling works */
    initial_render_complete = true;   
}

void rendering_engine::do_refresh_render()
{
    if (initial_render_complete) {
        /* Get width and height of tile graphics */
        int img_width = resources.get_terrain_img.at(
            parent->map.get_tile(0, 0)->get_terrain()).width();
        int img_height = resources.get_terrain_img.at(
            parent->map.get_tile(0, 0)->get_terrain()).height();

        painter.begin(&pixmap);
        for (int y = 0; y <= pixmap.height() + padding_top / img_height
            && y < parent->map.get_height(); y++) {
            for (int x = 0; x <= pixmap.width() + padding_left / img_width
                && x < parent->map.get_width(); x++) {
                /* Draw tile at x, y where (x|y) % 32 = 0 */
                draw_tile(x * img_width - padding_left, y * img_height - padding_top);
            }
        }
        painter.end();

        parent->map_placeholder_set_pixmap(pixmap);
    }
}

void rendering_engine::on_window_resized(const QScrollArea& resizee)
{
    /* ResizeEvent gets also called as the window shows up,
    so to prevent resizeEvent from messing up the initial_render,
    bool initial_render_completed exists*/
    if (initial_render_complete) {
        int old_width = pixmap.width();
        int old_height = pixmap.height();

        int width, height;

        /* Adjust paddings for when you e.g. scrolled all the way to the right
        and maximized the window */
        padding_top = std::max(0, std::min(padding_top, 
            padding_top - (resizee.height() - old_height)));
        padding_left = std::max(0, std::min(padding_left, 
            padding_left - (resizee.width() - old_width)));

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

        pixmap = QPixmap(width, height);

        do_refresh_render();

        scrollarea_width_before_scroll = resizee.width();
        scrollarea_height_before_scroll = resizee.height();

        // Display the fresh image
        parent->adjust_scrollbars(padding_left, padding_top);
    }
}

void rendering_engine::on_vertical_scroll(int value)
{
    if (initial_render_complete) {
        padding_top = value;
        do_refresh_render();
    }
}

void rendering_engine::on_horizontal_scroll(int value)
{
    if (initial_render_complete) {
        padding_left = value;
        do_refresh_render();
    }
}