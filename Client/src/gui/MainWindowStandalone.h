#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "MainWindow.h"

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    MainWindowStandalone(Controller::MainController *controller);
private:
    void initializePluginFinder();
protected slots:
    void on_errorConnectingToServer(QString msg);
};

#endif // MAINFRAMEVST_H
