#include <QApplication>
#include "Service.h"

int main(int argc, char* args[])
{

    QApplication application(argc, args);

    QTimer timer(&application);
         timer.connect(&timer, SIGNAL(timeout()), &application, SLOT(quit()));
         timer.start(60000);

    Ninjam::Service* service = new Ninjam::Service();

    QStringList channels("channel test");
    service->startServerConnection("localhost", 2049, "test", channels);

    return application.exec();

}


