 #pragma once
#pragma once
#include <vector>
#include <fstream>
#include <exception>

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
};

class tga {
private:
	tga_header header;
	std::vector<rgba_pixel> pixel;

public:
	/*
		data_size holds the size of the image data in byte
	*/
	int data_size;

	tga(
		tga_header&& _header, std::vector<rgba_pixel>&& _pixel);

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
};