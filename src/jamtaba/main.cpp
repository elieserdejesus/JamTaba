#include "gui/mainframe.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <stdexcept>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try{
        MainFrame w;
        w.show();
        return a.exec();
    }
    catch(const std::runtime_error& ex){
        qDebug() << ex.what();
    }
    return 0;

}
