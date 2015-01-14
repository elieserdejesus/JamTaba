#include "gui/mainframe.h"
#include <QtWidgets/QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    MainFrame w;
    w.show();

	return a.exec();
}
