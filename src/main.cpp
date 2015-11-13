#include <QApplication>
#include <QMainWindow>
#include <QDir>
#include "StandAloneMainController.h"
#include "persistence/Settings.h"
#include "MainWindowStandalone.h"
#include "log/logging.h"

#include "Libs/SingleApplication/singleapplication.h"
#include "configurator.h"

extern Configurator *JTBConfig=NULL;

int main(int argc, char* args[] ){

    QApplication::setApplicationName("Jamtaba 2");
    QApplication::setApplicationVersion(APP_VERSION);
    //QApplication::setApplicationDisplayName("Jamtaba Standalone");

    //start the configurator
    JTBConfig=new Configurator();
    if(!JTBConfig->setUp(standalone)) qWarning() << "JTBConfig->setUp() FAILED !" ;

    //setup ini
    QString iniFilePath =JTBConfig->getIniFilePath();
    if(!iniFilePath.isEmpty()){
        qputenv("QT_LOGGING_CONF", QByteArray(iniFilePath.toUtf8()));
        qInstallMessageHandler(LogHandler);
    }

    Persistence::Settings settings;
    settings.load();

    SingleApplication* application = new SingleApplication(argc, args);

    Controller::StandaloneMainController mainController(settings, (QApplication*)application);
    mainController.configureStyleSheet("jamtaba.css");
    mainController.start();
    if(mainController.isUsingNullAudioDriver()){
        QMessageBox::about(nullptr, "Fatal error!", "Jamtaba can't detect any audio device in your machine!");
    }
    MainWindowStandalone  mainWindow(&mainController);
    mainController.setMainWindow(&mainWindow);

    mainWindow.show();

    //The SingleApplication class implements a showUp() signal. You can bind to that signal to raise your application's
    //window when a new instance had been started.
    QObject::connect(application, SIGNAL(showUp()), &mainWindow, SLOT(raise()));

    return application->exec();
 }

//++++++++++++++++++++++++++++++++++




