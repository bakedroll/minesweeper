#include <QApplication>
#include <QPointer>

#include "MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QPointer<MainWindow> window = new MainWindow();
    window->show();

    return app.exec();
}