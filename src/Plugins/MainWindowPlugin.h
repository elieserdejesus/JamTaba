#ifndef MAINWINDOWPLUGIN_H
#define MAINWINDOWPLUGIN_H

#include "gui/MainWindow.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"

class NinjamRoomWindowPlugin;

class MainWindowPlugin : public MainWindow
{
    Q_OBJECT

public:
    explicit MainWindowPlugin(MainControllerPlugin *mainController);

    inline MainControllerPlugin *getMainController() const override
    {
        return dynamic_cast<MainControllerPlugin *>(mainController);
    }

    TextEditorModifier *createTextEditorModifier() override;

protected:
    NinjamRoomWindow *createNinjamWindow(const login::RoomInfo &, controller::MainController *) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, const persistence::Subchannel &subChannel) override;

    void removeAllInputLocalTracks() override;

    PreferencesDialog * createPreferencesDialog() override;

    virtual inline QSize getMinimumWindowSize() const override { return PLUGIN_WINDOW_MIN_SIZE; }

    void resizeEvent(QResizeEvent *) override;

protected slots:
    void updateLocalInputChannelsGeometry() override;

private slots:

    void increaseWidth();
    void increaseHeight();
    void decreaseWidth();
    void decreaseHeight();

    void updateWindowSizeMenu();

private:
    bool firstChannelIsInitialized;

    bool canIncreaseWindowWidth() const;
    bool canIncreaseWindowHeight() const;
    bool canDecreaseWindowWidth() const;
    bool canDecreaseWindowHeight() const;

    QSize getZoomStepSize() const;
    QSize getMaxWindowSize() const;

    void initializeWindowSizeMenu();

    QAction *increaseWindowWidthAction;
    QAction *increaseWindowHeightAction;
    QAction *decreaseWindowWidthAction;
    QAction *decreaseWindowHeightAction;

    static const QSize PLUGIN_WINDOW_MIN_SIZE;

    static const quint32 ZOOM_STEP;

};

#endif // MAINWINDOWPLUGIN_H
