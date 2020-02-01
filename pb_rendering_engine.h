#pragma once
#include <pb_img.h>
#include <pb_map.h>
#include <pb_main_window.h>

#include <QObject>
#include <QImage>
#include <QScrollArea>
#include <QString>

class rendering_engine : public QObject 
{
	Q_OBJECT

public:
	img_resources resources;
	QImage qimg;
	bool game_is_on;

	rendering_engine(main_window* parent_,
		const std::string& path_to_resources = ".");
	rendering_engine(const rendering_engine&) = default;

	int get_full_width_px();
	int get_full_height_px();

	void render_pixel(int x, int y);
	void connect_to_parent();

public slots:
	void do_initial_render();
	void do_refresh_render();
	void on_window_resized(const QScrollArea& resizee);
	void on_vertical_scroll(int value);
	void on_horizontal_scroll(int value);

private:
	bool is_path_debug_mode;

	bool initial_render_complete;
	int h_value_before_scroll;
	int v_value_before_scroll;
	int scrollarea_width_before_scroll;
	int scrollarea_height_before_scroll;
	int padding_top;
	int padding_left;

	main_window* parent;
};

