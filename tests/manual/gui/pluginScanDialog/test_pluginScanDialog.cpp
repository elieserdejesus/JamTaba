#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <PluginScanDialog.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PluginScanDialog dialog;
    dialog.setCurrentScaning("Testing a big, very big plugins search path to see if the scanner window will grow or not ..., Testing a big, very big plugins search path to see if the scanner window will grow or not ...");

    dialog.show();

    return app.exec();
}
