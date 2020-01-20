#pragma once
#include "pb_map.h"

#include <vector>
#include <fstream>
#include <exception>
#include <string>
#include <memory>

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
	static std::unique_ptr<tga> load_file(std::ifstream& file);

	rgba_pixel get_pixel(int x, int y) const;
	void set_pixel(const rgba_pixel& pixel, int x, int y);
};

class img_resources {
protected:
	std::string path;
	std::string tile_folder;
	std::string robot_folder;

	// Umgebungen
	std::shared_ptr<tga> boden;
	std::shared_ptr<tga> boden_start_gegner;
	std::shared_ptr<tga> boden_start_patchbot;
	std::shared_ptr<tga> gefahr_abgrund;
	std::shared_ptr<tga> gefahr_wasser;
	std::shared_ptr<tga> hauptserver;
	std::shared_ptr<tga> hindernis_aliengras;
	std::shared_ptr<tga> hindernis_geheimgang;
	std::shared_ptr<tga> hindernis_schotter;
	std::shared_ptr<tga> tuer_automatisch_geschlossen;
	std::shared_ptr<tga> tuer_automatisch_offen;
	std::shared_ptr<tga> tuer_manuell_geschlossen;
	std::shared_ptr<tga> tuer_manuell_offen;
	std::shared_ptr<tga> wand_beton;
	std::shared_ptr<tga> wand_fels;

	// Roboter
	std::shared_ptr<tga> dead;
	std::shared_ptr<tga> patchbot;
	std::shared_ptr<tga> typ1_bugger;
	std::shared_ptr<tga> typ2_pusher;
	std::shared_ptr<tga> typ3_digger;
	std::shared_ptr<tga> typ4_swimmer;
	std::shared_ptr<tga> typ5_follower;
	std::shared_ptr<tga> typ6_hunter;
	std::shared_ptr<tga> typ7_sniffer;

public:
	img_resources(const std::string& _path, 
		const std::string& _tile_folder = "umgebungen",
		const std::string& _robot_folder = "roboter");

	/*
		get_tga() returns a ptr to the suiting image object
	*/
	std::shared_ptr<tga> get_tga(tile _tile) const;
	std::shared_ptr<tga> get_tga(robot _robot) const;
};