#include <QApplication>

#include "gui/IntervalProgressDisplay.h"
#include "TestMainWindow.h"
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMainWindow window;

    window.show();
    return app.exec();
}
