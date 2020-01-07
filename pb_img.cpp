#include "pb_img.h"

#include <stdexcept>

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

tga::tga(tga_header&& _header,
	std::vector <rgba_pixel>&& _pixel) :
	header(_header),
	pixel(_pixel),
	data_size(_pixel.size() * 4)
{
	if (_header.img_width * _header.img_height
		!= _pixel.size())
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
	for (rgba_pixel p : pixel) {
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

tga tga::load_file(std::ifstream& file) {
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

	return tga(std::move(header), std::move(pixel_data));
}