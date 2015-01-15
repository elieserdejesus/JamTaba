#include "gui/mainframe.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <exception>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try{
        MainFrame w;
        w.show();
        return a.exec();
    }
    catch(const std::exception& ex){
        qDebug() << ex.what();
    }


}
