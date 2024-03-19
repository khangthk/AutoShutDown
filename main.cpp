#include "autoshutdown.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoShutdown w;
    w.show();

    return a.exec();
}
