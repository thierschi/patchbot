#include "pb_main_window.h"
#include "pb_io.h"
#include "pb_map.h"
#include "pb_img.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    main_window w;
    w.show();
    w.initial_render();

    return a.exec();
}