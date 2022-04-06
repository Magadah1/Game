#include "Game.h"
#include "MainGame.h"
#include "Game3D.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*Game w;
    w.show();*/

    //MainGame w;
    //w.show();

    Game3D g;
    g.show();

    return a.exec();
}
