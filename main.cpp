#include "pb_main_window.h"
#include "pb_io.h"
#include "pb_map.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    std::ofstream out("logfile.txt");
    std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    try {
        tile_map map = pb_input::read_map_txt("..\\txt\\everything.txt");
        pb_output::print_map(map);
        pb_output::print_map(map.get_robot_map());
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
    

    std::cout << sizeof(robot);

    std::cout.rdbuf(coutbuf); //reset to standard output again

    /*QApplication a(argc, argv);
    main_window w;
    w.show();
    return a.exec();*/
}