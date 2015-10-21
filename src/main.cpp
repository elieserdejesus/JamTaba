#include <QApplication>
#include <QMainWindow>
#include "StandAloneMainController.h"
#include "persistence/Settings.h"
#include "MainWindowStandalone.h"
#include "log/logHandler.h"
//#include <QDebug>
//#include <QStandardPaths>

int main(int argc, char* args[] ){

    //qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    QApplication::setApplicationName("Jamtaba 2");
    QApplication::setApplicationVersion(APP_VERSION);

    qputenv("QT_LOGGING_CONF", ":/qtlogging.ini");//log cconfigurations is in resources at moment
    qInstallMessageHandler(customLogHandler);

    Persistence::Settings settings;//read from file in constructor
    settings.load();

    QApplication* application = new QApplication(argc, args);
    Controller::StandaloneMainController mainController(settings, application);//MainController extends QApplication
    mainController.configureStyleSheet("jamtaba.css");
    mainController.start();
    if(mainController.getAudioDriver()->getDevicesCount() <= 0){
        QMessageBox::about(nullptr, "Fatal error!", "Jamtaba can't detect any audio device in your machine!");
        application->quit();
    }
    MainWindowStandalone  mainWindow(&mainController);
    mainController.setMainWindow(&mainWindow);
    mainWindow.show();

    return application->exec();
 }

//++++++++++++++++++++++++++++++++++




