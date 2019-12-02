#include "pb_io.h"
#include "pb_img.h"

#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] 
			<< " [path to input-.txt-file] "
			"[path to output-.txt-file]" << std::endl;
		return 0;
	}
	const char* i_path = argv[1];
	const char* o_path = argv[2];

	/*const char* i_path = "..\\..\\roboter\\patchbot.tga";
	const char* o_path = "..\\..\\roboter\\dead_out.tga";*/

	try {
		std::cout << "Reading image from TARGA file at " 
			<< i_path << "... " << std::endl;
		tga img = pb_input::read_tga_img(i_path);
		std::cout << "Read image successfully." 
			<< std::endl << std::endl;

		std::cout << "Writing TARGA image file to " 
			<< o_path << "... " << std::endl;
		pb_output::write_tga_img(o_path, img);
		std::cout << "Wrote image successfully." 
			<< std::endl;
	}
	catch (const std::exception & e) {
		std::cout << std::endl << "Exception " 
			<< e.what() << std::endl;
	}

	return 0;
}