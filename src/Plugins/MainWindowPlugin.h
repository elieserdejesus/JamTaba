#ifndef MAINWINDOWVST_H
#define MAINWINDOWVST_H

#include "gui/MainWindow.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"

class NinjamRoomWindowPlugin;

class MainWindowPlugin : public MainWindow
{
    Q_OBJECT

public:
    MainWindowPlugin(MainControllerPlugin *mainController);

    inline MainControllerPlugin *getMainController() override
    {
        return dynamic_cast<MainControllerPlugin *>(mainController);
    }

protected:
    NinjamRoomWindow *createNinjamWindow(const Login::RoomInfo &, Controller::MainController *) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel) override;

    void removeAllInputLocalTracks() override;

    PreferencesDialog * createPreferencesDialog() override;

    void initializeWindowMinimumSize() override;

private slots:

    void zoomIn();
    void zoomOut();

private:
    bool firstChannelIsInitialized;

    void initializeWindowSizeControls();

    static const QSize PLUGIN_WINDOW_MIN_SIZE;

    static const quint32 ZOOM_STEP;

};

#endif // MAINWINDOWVST_H
