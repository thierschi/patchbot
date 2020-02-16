#pragma once
#include "pb_map.h"

#include <vector>
#include <fstream>
#include <exception>
#include <string>
#include <memory>
#include <unordered_map>
#include <QPixmap>

class img_exception : public std::exception {
private:
	const char* message;

public:
	explicit img_exception(const char* message_);
	virtual const char* what() const throw();
};

struct tga_header {
	char id_length;
	char color_map_type;
	char img_type;
	short color_map_start;
	short color_map_length;
	char color_map_size;
	short x_origin;
	short y_origin;
	short img_width;
	short img_height;
	char pixel_depth;
	char img_descriptor;
};

class rgba_pixel {
public:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;

	rgba_pixel(
		char r_ = 0,
		char g_ = 0,
		char b_ = 0,
		char a_ = 0);

	/* Blends given pixel over this pixel*/
	void overlay_pixel(const rgba_pixel& pixel);

	/* Blends this pixle over the given one*/
	void underlay_pixel(const rgba_pixel& pixel);

	/* Blends overlay pixel over underlay_pixel and returns resulting pixel */
	static rgba_pixel blend(const rgba_pixel& overlay_pixel, 
		const rgba_pixel& underlay_pixel);

};

class tga {
private:
	std::vector<rgba_pixel> pixel_map;

public:
	tga_header header;

	/*
		data_size holds the size of the image data in byte
	*/
	int data_size;

	tga();
	tga(
		tga_header&& header_, std::vector<rgba_pixel>&& pixel_map_);

	/*
		Returns smart ptr to char array which holds the binary data for the
		tga image that can be written into a file.
	*/
	std::unique_ptr<char[]> get_raw_data() const;

	/*
		Takes an open file (ifstream binary) as an argument and creats a tga
		object from the binary data in the file and returns that object
	*/
	static tga load_file(std::ifstream& file);

	/*
		Convert tga to QImage
	*/
	QPixmap to_qpixmap() const;

	rgba_pixel get_pixel(int x, int y) const;
	void set_pixel(const rgba_pixel& pixel, int x, int y);
};

class img_resources {
protected:
	std::string path;
	std::string tile_folder;
	std::string robot_folder;
	std::string arrow_folder;

public:
	std::unordered_map<terrain, QPixmap> get_terrain_img;
	std::unordered_map<robot_type, QPixmap> get_robot_img;
	std::unordered_map<direction, QPixmap> get_arrow_img;

	img_resources(const std::string& path_,
		const std::string& tile_folder_ = "umgebungen",
		const std::string& robot_folder_ = "roboter",
		const std::string& arrow_folder_ = "pfeile");
};