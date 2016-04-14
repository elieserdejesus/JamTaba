#include <QApplication>
#include <QMainWindow>
#include <QDir>

#include "MainControllerStandalone.h"
#include "gui/MainWindowStandalone.h"
#include "persistence/Settings.h"
#include "log/Logging.h"
#include "SingleApplication/singleapplication.h"
#include "Configurator.h"

int main(int argc, char* args[] ){

    QApplication::setApplicationName("Jamtaba 2");
    QApplication::setApplicationVersion(APP_VERSION);

    //start the configurator
    Configurator* configurator = Configurator::getInstance();
    if(!configurator->setUp(STANDALONE)) qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !" ;

    Persistence::Settings settings;
    settings.load();

//SingleApplication is not working in mac. Using a dirty ifdef until have time to solve the SingleApplication issue in Mac
#ifdef Q_OS_WIN
    QApplication* application = new SingleApplication(argc, args);
#else
    QApplication* application = new QApplication(argc, args);
#endif

    Controller::MainControllerStandalone mainController(settings, (QApplication*)application);
    mainController.configureStyleSheet("jamtaba.css");
    mainController.start();
    if(mainController.isUsingNullAudioDriver()){
        QMessageBox::about(nullptr, "Fatal error!", "Jamtaba can't detect any audio device in your machine!");
    }
    MainWindowStandalone mainWindow(&mainController);
    mainController.setMainWindow(&mainWindow);
    mainWindow.initialize();

    mainWindow.show();

#ifdef Q_OS_WIN
    //The SingleApplication class implements a showUp() signal. You can bind to that signal to raise your application's
    //window when a new instance had been started.
    QObject::connect(application, SIGNAL(showUp()), &mainWindow, SLOT(raise()));
#endif
    int execResult = application->exec();
    mainController.saveLastUserSettings(mainWindow.getInputsSettings());
    return execResult;
 }

//++++++++++++++++++++++++++++++++++




