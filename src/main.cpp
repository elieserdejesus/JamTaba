#include "gui/MainWindowStandalone.h"
#include "StandAloneMainController.h"
#include "persistence/Settings.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QDir>
#include <QStandardPaths>
#include "log/logHandler.h"

int main(int argc, char* args[] ){

    QApplication::setApplicationName("Jamtaba 2");
    QApplication::setApplicationVersion(APP_VERSION);

    qputenv("QT_LOGGING_CONF", ":/qtlogging.ini");//log cconfigurations is in resources at moment
    qInstallMessageHandler(customLogHandler);

    Persistence::Settings settings;//read from file in constructor
    settings.load();

    Controller::StandaloneMainController mainController(settings, argc, args);//MainController extends QApplication
    mainController.configureStyleSheet("jamtaba.css");
    mainController.start();
    MainWindowStandalone  mainWindow(&mainController);
    mainController.setMainWindow(&mainWindow);
    mainWindow.show();

    int returnCode = mainController.exec();
    mainController.saveLastUserSettings(mainWindow.getInputsSettings());

    return returnCode;

 }

//++++++++++++++++++++++++++++++++++




