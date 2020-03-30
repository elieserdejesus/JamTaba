#include <QApplication>
#include <QMainWindow>
#include <QDir>

#include "MainControllerStandalone.h"
#include "gui/MainWindowStandalone.h"
#include "persistence/Settings.h"
#include "log/Logging.h"
#include "SingleApplication/singleapplication.h"
#include "Configurator.h"

int main(int argc, char *args[])
{
    QApplication::setApplicationName("JamTaba 2");
    QApplication::setApplicationVersion(APP_VERSION);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // fixing issue https://github.com/elieserdejesus/JamTaba/issues/1216

    auto configurator = Configurator::getInstance();
    if (!configurator->setUp())
        qCritical() << "JTBConfig->setUp() FAILED !";

// SingleApplication is not working in mac. Using a dirty ifdef until have time to solve the SingleApplication issue in Mac
#ifdef Q_OS_WIN
    SingleApplication application(argc, args);
#else
    QApplication application(argc, args);
#endif

    application.setStyle("fusion"); // same visual in all platforms

    persistence::Settings settings;
    settings.load();

    controller::MainControllerStandalone mainController(settings, &application);
    mainController.start();

    if (mainController.isUsingNullAudioDriver())
        QMessageBox::about(nullptr, "Fatal error!", "Jamtaba can't detect any audio device in your machine!");

    MainWindowStandalone mainWindow(&mainController);
    mainController.setMainWindow(&mainWindow);

    mainWindow.initialize();
    mainWindow.show();

#ifdef Q_OS_WIN
    // The SingleApplication class implements a showUp() signal. You can bind to that signal to raise your application's
    // window when a new instance had been started.
    QObject::connect(&application, &SingleApplication::showUp, &mainWindow, &MainWindow::raise);
#endif
    int execResult = application.exec();

    mainController.saveLastUserSettings(mainWindow.getInputsSettings());

    return execResult;
}
