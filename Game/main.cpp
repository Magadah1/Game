#include "Game.h"
#include "MainGame.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Game w;
    w.show();

    //MainGame w;
    //w.show();
    return a.exec();
}
