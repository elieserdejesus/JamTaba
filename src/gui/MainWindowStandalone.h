#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "MainWindow.h"

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    MainWindowStandalone(Controller::MainController *controller);

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

    virtual NinjamRoomWindow* createNinjamWindow(Login::RoomInfo, Controller::MainController*);
private:
    void initializePluginFinder();
protected slots:
    void on_errorConnectingToServer(QString msg);
};

#endif // MAINFRAMEVST_H
