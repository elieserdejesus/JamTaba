#include "AutoUpdaterDialog.h"
#include <QDesktopWidget>
#include "AutoUpdater.h"
#include "../src/log/logHandler.h"

int main(int argc, char *argv[]){

    qputenv("QT_LOGGING_CONF", ":/log.ini");//log cconfigurations is in resources bundle
    qInstallMessageHandler(customLogHandler);



    AutoUpdater autoUpdater(argc, argv);
    AutoUpdaterDialog view;
    autoUpdater.setView(&view);
    view.show();

    QRect scr = QApplication::desktop()->screenGeometry();
    view.move( scr.center() - view.rect().center() );

    autoUpdater.start();

    return autoUpdater.exec();
}
