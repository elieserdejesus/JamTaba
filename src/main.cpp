#include <QApplication>
#include <QMainWindow>
#include <QDir>
#include "StandAloneMainController.h"
#include "persistence/Settings.h"
#include "MainWindowStandalone.h"
#include "log/logging.h"
#include "Libs/SingleApplication/singleapplication.h"
#include "configurator.h"

int main(int argc, char* args[] ){

    QApplication::setApplicationName("Jomtobo");
    QApplication::setApplicationVersion(APP_VERSION);

    //start the configurator
    Configurator* configurator = Configurator::getInstance();
    if(!configurator->setUp(standalone)) qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !" ;

    Persistence::Settings settings;
    settings.load();

    SingleApplication* application = new SingleApplication(argc, args);

    Controller::StandaloneMainController mainController(settings, (QApplication*)application);
    //..JOMTOBO CSS
    mainController.configureStyleSheet("jomtobo.css");
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




