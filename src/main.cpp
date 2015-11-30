#include <QApplication>
#include <QMainWindow>
#include <QDir>
#include "StandAloneMainController.h"
#include "persistence/Settings.h"
#include "gui/MainWindowStandalone.h"
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

//SingleApplication is not working in mac. Using a dirty ifdef until have time to solve the SingleApplication issue in Mac
#ifdef Q_OS_WIN
    QApplication* application = new SingleApplication(argc, args);
#else
    QApplication* application = new QApplication(argc, args);
#endif

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

#ifdef Q_OS_WIN
    //The SingleApplication class implements a showUp() signal. You can bind to that signal to raise your application's
    //window when a new instance had been started.
    QObject::connect(application, SIGNAL(showUp()), &mainWindow, SLOT(raise()));
#endif
    return application->exec();

 }

//++++++++++++++++++++++++++++++++++




