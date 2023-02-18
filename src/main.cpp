#include "mainWindow.h"
#include <QApplication>
#include <QImageReader>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QImageReader::setAllocationLimit(0);

    MainWindow::instance();

    return app.exec();
}