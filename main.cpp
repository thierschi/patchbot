#include "pb_main_window.h"
#include "pb_io.h"
#include "pb_map.h"
#include "pb_img.h"
#include "pb_rendering_engine.h"
#include "pb_controls.h"
#include "pb_game_logic.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    main_window w;
    rendering_engine r(&w);
    controls c(&w);
    game_logic l(&w, &r, &c);
    w.show();
    r.initial_render();
    return a.exec();
}