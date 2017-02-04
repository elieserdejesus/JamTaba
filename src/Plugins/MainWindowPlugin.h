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

    TextEditorModifier *createTextEditorModifier() override;

protected:
    NinjamRoomWindow *createNinjamWindow(const Login::RoomInfo &, Controller::MainController *) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel) override;

    void removeAllInputLocalTracks() override;

    PreferencesDialog * createPreferencesDialog() override;

    virtual inline QSize getMinimumWindowSize() const override { return PLUGIN_WINDOW_MIN_SIZE; }

    void resizeEvent(QResizeEvent *) override;

#ifndef Q_OS_MAC  // this function is NOT used in Mac
    void timerEvent(QTimerEvent *) override;
#endif

protected slots:
    void updateLocalInputChannelsGeometry() override;

private slots:

    void zoomIn();
    void zoomOut();

    void updateWindowSizeMenu();

private:
    bool firstChannelIsInitialized;

    bool canDecreaseWindowSize() const;
    bool canIncreaseWindowSize() const;
    QSize getZoomStepSize() const;
    QSize getMaxWindowSize() const;

    void initializeWindowSizeMenu();

    QAction *increaseWindowSizeAction;
    QAction *decreaseWindowSizeAction;

    static const QSize PLUGIN_WINDOW_MIN_SIZE;

    static const quint32 ZOOM_STEP;

};

#endif // MAINWINDOWVST_H
