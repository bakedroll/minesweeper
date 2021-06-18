#include <QApplication>
#include <QPointer>
#include <QTranslator>

#include "MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("QtMinesweeper"), QLatin1String("_"), QLatin1String(":/translations")))
    {
      QApplication::installTranslator(&translator);
    }

    QPointer<MainWindow> window = new MainWindow();
    window->show();

    return app.exec();
}