#include "Game3D.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Game3D g;
    g.show();

    return a.exec();
}
