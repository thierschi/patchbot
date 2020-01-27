#pragma once
#include "pb_map.h"

#include <vector>
#include <fstream>
#include <exception>
#include <string>
#include <memory>
#include <unordered_map>

class img_exception : public std::exception {
private:
	const char* message;

public:
	explicit img_exception(const char* _message);
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
		char _r = 0,
		char _g = 0,
		char _b = 0,
		char _a = 0);

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

	tga(
		tga_header&& _header, std::vector<rgba_pixel>&& _pixel_map);

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

	rgba_pixel get_pixel(int x, int y) const;
	void set_pixel(const rgba_pixel& pixel, int x, int y);
};

class img_resources {
protected:
	std::string path;
	std::string tile_folder;
	std::string robot_folder;

public:
	std::unordered_map<terrain, tga> get_terrain_img;
	std::unordered_map<robot_type, tga> get_robot_img;

	img_resources(const std::string& _path,
		const std::string& _tile_folder = "umgebungen",
		const std::string& _robot_folder = "roboter");
};