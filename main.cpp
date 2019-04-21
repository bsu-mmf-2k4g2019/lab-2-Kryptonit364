#include "glwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GLWidget w;

    w.resize(640, 640);
    w.show();
    w.startTimer(20);

    return a.exec();
}
