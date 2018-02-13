#include <QApplication>

#include "gui/PrivateServerWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PrivateServerWindow window;

    window.show();
    return app.exec();
}
