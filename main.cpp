#include <QApplication>
#include <QFile>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w(argc>1 && QFile().exists(argv[1]) ? argv[1] : "");
    w.show();
    return app.exec();
}
