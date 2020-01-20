#include "pb_img.h"
#include "pb_io.h"

#include <stdexcept>
#include <math.h>
#include <iostream>

img_exception::img_exception(const char* _message) :
	message(_message)
{
}

const char* img_exception::what() const throw() {
	return message;
}

rgba_pixel::rgba_pixel(
	char _red,
	char _green,
	char _blue,
	char _alpha) :
	red(_red),
	green(_green),
	blue(_blue),
	alpha(_alpha)
{
}

void rgba_pixel::overlay_pixel(const rgba_pixel& pixel)
{
	/*
		https://en.wikipedia.org/wiki/Alpha_compositing
	*/

	// Alpha
	double alpha_p = alpha / 255.0;
	double o_alpha_p = pixel.alpha / 255.0;
	double new_alpha = o_alpha_p + (1 - o_alpha_p) * alpha_p;
	alpha = floor(new_alpha * 255.0);

	// Red
	double red_p = red / 255.0;
	double o_red_p = pixel.red / 255.0;
	red = floor(
		(
			(1 / new_alpha) * 
			(o_alpha_p * o_red_p + (1 - o_alpha_p) * alpha_p * red_p)
		) * 255.0
	);

	// Green
	double green_p = green / 255.0;
	double o_green_p = pixel.green / 255.0;
	green = floor(
		(
			(1 / new_alpha) *
			(o_alpha_p * o_green_p + (1 - o_alpha_p) * alpha_p * green_p)
		) * 255.0
	);

	// Blue
	double blue_p = blue / 255.0;
	double o_blue_p = pixel.blue / 255.0;
	blue = floor(
		(
			(1 / new_alpha) *
			(o_alpha_p * o_blue_p + (1 - o_alpha_p) * alpha_p * blue_p)
		) * 255.0
	);
}
	
void rgba_pixel::underlay_pixel(const rgba_pixel& pixel)
{
	/*
		https://en.wikipedia.org/wiki/Alpha_compositing
	*/

	// Alpha
	double alpha_p = pixel.alpha / 255.0;
	double o_alpha_p = alpha / 255.0;
	double new_alpha = o_alpha_p + (1 - o_alpha_p) * alpha_p;
	alpha = floor(new_alpha * 255.0);

	// Red
	double red_p = pixel.red / 255.0;
	double o_red_p = red / 255.0;
	red = floor(
		(
		(1 / new_alpha) *
			(o_alpha_p * o_red_p + (1 - o_alpha_p) * alpha_p * red_p)
		) * 255.0
	);

	// Green
	double green_p = pixel.green / 255.0;
	double o_green_p = green / 255.0;
	green = floor(
		(
		(1 / new_alpha) *
			(o_alpha_p * o_green_p + (1 - o_alpha_p) * alpha_p * green_p)
		) * 255.0
	);

	// Blue
	double blue_p = pixel.blue / 255.0;
	double o_blue_p = blue / 255.0;
	blue = floor(
		(
		(1 / new_alpha) *
			(o_alpha_p * o_blue_p + (1 - o_alpha_p) * alpha_p * blue_p)
		) * 255.0
	);
}

rgba_pixel rgba_pixel::blend(const rgba_pixel& overlay_pixel, 
	const rgba_pixel& underlay_pixel)
{
	/*
		https://en.wikipedia.org/wiki/Alpha_compositing
	*/

	// Alpha
	double alpha_p = underlay_pixel.alpha / 255.0;
	double o_alpha_p = overlay_pixel.alpha / 255.0;
	double new_alpha = o_alpha_p + (1 - o_alpha_p) * alpha_p;

	// Red
	double red_p = underlay_pixel.red / 255.0;
	double o_red_p = overlay_pixel.red / 255.0;
	double red = (1 / new_alpha) *
		(o_alpha_p * o_red_p + (1 - o_alpha_p) * alpha_p * red_p);

	// Green
	double green_p = underlay_pixel.green / 255.0;
	double o_green_p = overlay_pixel.green / 255.0;
	double green = (1 / new_alpha) *
		(o_alpha_p * o_green_p + (1 - o_alpha_p) * alpha_p * green_p);

	// Blue
	double blue_p = underlay_pixel.blue / 255.0;
	double o_blue_p = overlay_pixel.blue / 255.0;
	double blue = (1 / new_alpha) *
		(o_alpha_p * o_blue_p + (1 - o_alpha_p) * alpha_p * blue_p);

	return rgba_pixel(floor(red * 255.0), 
		floor(green * 255.0), 
		floor(blue * 255.0), 
		floor(new_alpha * 255.0));
}

tga::tga(tga_header&& _header,
	std::vector <rgba_pixel>&& _pixel_map) :
	header(_header),
	pixel_map(_pixel_map),
	data_size(_pixel_map.size() * 4)
{
	if (_header.img_width * _header.img_height
		!= _pixel_map.size())
		throw std::invalid_argument("Cannot construct tga image class: "
			"Pixel vector's size does not match "
			"width * height specified in header.");
}

std::unique_ptr<char[]> tga::get_raw_data() const {
	// 44 is the size of header (18) and footer (26)
	int size = 44 + data_size;
	std::unique_ptr<char[]> raw_data = std::make_unique<char[]>(size);

	/*
		Convert header
		temp is used for conversion of short int to char[2]
	*/
	char* temp;
	raw_data[0] = header.id_length;
	raw_data[1] = header.color_map_type;
	raw_data[2] = header.img_type;
	temp = (char*)&header.color_map_start;
	raw_data[3] = temp[0];
	raw_data[4] = temp[1];
	temp = (char*)&header.color_map_length;
	raw_data[5] = temp[0];
	raw_data[6] = temp[1];
	raw_data[7] = header.color_map_size;
	temp = (char*)&header.x_origin;
	raw_data[8] = temp[0];
	raw_data[9] = temp[1];
	temp = (char*)&header.y_origin;
	raw_data[10] = temp[0];
	raw_data[11] = temp[1];
	temp = (char*)&header.img_width;
	raw_data[12] = temp[0];
	raw_data[13] = temp[1];
	temp = (char*)&header.img_height;
	raw_data[14] = temp[0];
	raw_data[15] = temp[1];
	raw_data[16] = header.pixel_depth;
	raw_data[17] = header.img_descriptor;

	// Store binary image data
	int b = 18;
	for (rgba_pixel p : pixel_map) {
		// Convert from rgba_pixels back to chars
		raw_data[b] = p.blue;
		raw_data[b + 1] = p.green;
		raw_data[b + 2] = p.red;
		raw_data[b + 3] = p.alpha;
		b += 4;
	}

	// Write footer
	for (int i = 0; i < 8; i++) {
		raw_data[b] = 0;
		b++;
	}
	char truevision_xfile[] = "TRUEVISION-XFILE";
	for (int i = 0; i < 16; i++) {
		raw_data[b] = truevision_xfile[i];
		b++;
	}
	raw_data[b++] = '.';
	raw_data[b] = 0;

	return raw_data;
}

std::unique_ptr<tga> tga::load_file(std::ifstream& file) {
	if (!file.is_open())
		throw std::invalid_argument(
			"Image file exception: File was not opened.");

	// Get file size
	file.seekg(0, std::ios::end);
	int file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	tga_header header;

	// Read header attributes
	file.read(&header.id_length,
		sizeof(header.id_length));
	file.read(&header.color_map_type,
		sizeof(header.color_map_type));
	file.read(&header.img_type,
		sizeof(header.img_type));
	file.read((char*)(&header.color_map_start),
		sizeof(header.color_map_start));
	file.read((char*)(&header.color_map_length),
		sizeof(header.color_map_length));
	file.read(&header.color_map_size,
		sizeof(header.color_map_size));
	file.read((char*)(&header.x_origin),
		sizeof(header.x_origin));
	file.read((char*)(&header.y_origin),
		sizeof(header.y_origin));
	file.read((char*)(&header.img_width),
		sizeof(header.img_width));
	file.read((char*)(&header.img_height),
		sizeof(header.img_height));
	file.read(&header.pixel_depth,
		sizeof(header.pixel_depth));
	file.read(&header.img_descriptor,
		sizeof(header.img_descriptor));

	// Check for support and throw exception if unsupported
	if (header.color_map_type != 0)
		throw img_exception("TGA image unsupported: "
			"Only color map type 0 is supported.");
	if (header.img_type != 2)
		throw img_exception("TGA image unsupported: "
			"Only image type 2 is supported.");
	if (header.x_origin != 0 || header.y_origin != 0)
		throw img_exception("TGA image unsupported: "
			"Only x and y origin values euqal to zero "
			"are supported.");
	if (header.pixel_depth != 32)
		throw img_exception("TGA image unsupported: "
			"Only images with a pixel depth of 32 are "
			"supported.");

	// Check for too short files
	int min_file_size = 18 + header.id_length
		+ header.img_width * header.img_height
		* (header.pixel_depth / 8);
	if (file_size < min_file_size)
		throw img_exception("TGA image corrupted: File is too short.");

	// Skip ID if present
	if (header.id_length != 0) {
		file.ignore(header.id_length);
	}

	int img_data_size = header.img_width * header.img_height *
		(header.pixel_depth / 8);
	std::unique_ptr<char[]> pixel_raw_data =
		std::make_unique<char[]>(img_data_size);

	file.read(pixel_raw_data.get(), img_data_size);
	// Tidy up after ourselves
	file.seekg(0, std::ios::beg);

	std::vector<rgba_pixel> pixel_data;

	for (int i = 0; i < img_data_size; i += 4) {
		/*
			Convert binary data to rgba pixels
			rgba values are stored in order BGRA
		*/
		pixel_data.push_back(rgba_pixel(
			pixel_raw_data[i + 2],
			pixel_raw_data[i + 1],
			pixel_raw_data[i],
			pixel_raw_data[i + 3]
		));
	}

	return std::make_unique<tga>(std::move(header), std::move(pixel_data));
}

rgba_pixel tga::get_pixel(int x, int y) const {
	if (x >= header.img_width || y >= header.img_height)
		throw std::invalid_argument("Invalid argument passed to tga: "
			"Coordinates out of range.");
	return pixel_map[y * header.img_width + x];
}

void tga::set_pixel(const rgba_pixel& pixel, int x, int y) {
	if (x >= header.img_width || y >= header.img_height)
		throw std::invalid_argument("Invalid argument passed to tga: "
			"Coordinates out of range.");
	pixel_map[y * header.img_width + x] = pixel;
}

img_resources::img_resources(const std::string& _path, 
	const std::string& _tile_folder, const std::string& _robot_folder) :
	path(_path),
	tile_folder(_tile_folder),
	robot_folder(_robot_folder)
{
	if (path.back() == '\\') path.pop_back();
	if (tile_folder.back() == '\\') tile_folder.pop_back();
	if (tile_folder.front() == '\\') tile_folder.erase(tile_folder.begin());
	if (robot_folder.back() == '\\') robot_folder.pop_back();
	if (robot_folder.front() == '\\') robot_folder.erase(tile_folder.begin());

	std::string path_tiles = path + '\\' + tile_folder;
	boden = pb_input::read_tga_img(path_tiles 
		+ "\\boden.tga");
	boden_start_gegner = pb_input::read_tga_img(path_tiles 
		+ "\\boden_start_gegner.tga");
	boden_start_patchbot = pb_input::read_tga_img(path_tiles 
		+ "\\boden_start_patchbot.tga");
	gefahr_abgrund = pb_input::read_tga_img(path_tiles 
		+ "\\gefahr_abgrund.tga");
	gefahr_wasser = pb_input::read_tga_img(path_tiles 
		+ "\\gefahr_wasser.tga");
	hauptserver = pb_input::read_tga_img(path_tiles 
		+ "\\hauptserver.tga");
	hindernis_aliengras = pb_input::read_tga_img(path_tiles 
		+ "\\hindernis_aliengras.tga");
	hindernis_geheimgang = pb_input::read_tga_img(path_tiles 
		+ "\\hindernis_geheimgang.tga");
	hindernis_schotter = pb_input::read_tga_img(path_tiles 
		+ "\\hindernis_schotter.tga");
	tuer_automatisch_geschlossen = pb_input::read_tga_img(path_tiles 
		+ "\\tuer_automatisch_geschlossen.tga");
	tuer_automatisch_offen = pb_input::read_tga_img(path_tiles 
		+ "\\tuer_automatisch_offen.tga");
	tuer_manuell_geschlossen = pb_input::read_tga_img(path_tiles 
		+ "\\tuer_manuell_geschlossen.tga");
	tuer_manuell_offen = pb_input::read_tga_img(path_tiles 
		+ "\\tuer_manuell_offen.tga");
	wand_beton = pb_input::read_tga_img(path_tiles 
		+ "\\wand_beton.tga");
	wand_fels = pb_input::read_tga_img(path_tiles 
		+ "\\wand_fels.tga");

	std::string path_robots = path + '\\' + robot_folder;
	dead = pb_input::read_tga_img(path_robots + "\\dead.tga");
	patchbot = pb_input::read_tga_img(path_robots + "\\patchbot.tga");
	typ1_bugger = pb_input::read_tga_img(path_robots + "\\typ1_bugger.tga");
	typ2_pusher = pb_input::read_tga_img(path_robots + "\\typ2_pusher.tga");
	typ3_digger = pb_input::read_tga_img(path_robots + "\\typ3_digger.tga");
	typ4_swimmer = pb_input::read_tga_img(path_robots + "\\typ4_swimmer.tga");
	typ5_follower = pb_input::read_tga_img(path_robots + "\\typ5_follower.tga");
	typ6_hunter = pb_input::read_tga_img(path_robots + "\\typ6_hunter.tga");
	typ7_sniffer = pb_input::read_tga_img(path_robots + "\\typ7_sniffer.tga");
}

std::shared_ptr<tga> img_resources::get_tga(tile _tile) const {
	switch (_tile.get_terrain()) {
	case(terrain::STEEL_PLANKS):
		return boden;
		break;
	case(terrain::BUGGER_START):
	case(terrain::PUSHER_START):
	case(terrain::DIGGER_START):
	case(terrain::SWIMMER_START):
	case(terrain::FOLLOWER_START):
	case(terrain::HUNTER_START):
	case(terrain::SNIFFER_START):
		return boden_start_gegner;
		break;
	case(terrain::PATCHBOT_START):
		return boden_start_patchbot;
		break;
	case(terrain::ABYSS):
		return gefahr_abgrund;
		break;
	case(terrain::WATER):
		return gefahr_wasser;
		break;
	case(terrain::MAIN_SERVER):
		return hauptserver;
		break;
	case(terrain::ALIEN_GRASS):
		return hindernis_aliengras;
		break;
	case(terrain::SECRET_PASSAGE):
		return hindernis_geheimgang;
		break;
	case(terrain::GRAVEL):
		return hindernis_schotter;
		break;
	case(terrain::AUTOMATIC_DOOR):
		if (_tile.get_is_open())
			return tuer_automatisch_offen;
		else
			return tuer_automatisch_geschlossen;
		break;
	case(terrain::MANUAL_DOOR):
		if (_tile.get_is_open())
			return tuer_manuell_offen;
		else
			return tuer_manuell_geschlossen;
		break;
	case(terrain::CONCRETE_WALL):
		return wand_beton;
		break;
	case(terrain::ROCK_WALL):
		return wand_fels;
		break;
	default:
		std::cout << (char)_tile.get_terrain() << "\n\n";
		throw std::invalid_argument("Resource exception: "
			"No resource available for given tile.");
		break;
	}
}

std::shared_ptr<tga> img_resources::get_tga(robot _robot) const {
	if (_robot.is_dead)
		return dead;
	switch (_robot.type) {
	case(robot_type::PATCHBOT):
		return patchbot;
		break;
	case(robot_type::BUGGER):
		return typ1_bugger;
		break;
	case(robot_type::PUSHER):
		return typ2_pusher;
		break;
	case(robot_type::DIGGER):
		return typ3_digger;
		break;
	case(robot_type::SWIMMER):
		return typ4_swimmer;
		break;
	case(robot_type::FOLLOWER):
		return typ5_follower;
		break;
	case(robot_type::HUNTER):
		return typ6_hunter;
		break;
	case(robot_type::SNIFFER):
		return typ7_sniffer;
		break;
	default:
		throw std::invalid_argument("Resource exception:"
			"No resource available for given robot.");
		break;
	}
}