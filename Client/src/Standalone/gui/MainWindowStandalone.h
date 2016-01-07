#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "MainWindow.h"
#include "LocalTrackGroupViewStandalone.h"
#include "LocalTrackViewStandalone.h"
#include "MainControllerStandalone.h"
#include "PluginScanDialog.h"

using namespace Controller;

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    MainWindowStandalone(MainControllerStandalone *controller);

    Persistence::LocalInputTrackSettings getInputsSettings() const override;

    void addChannelsGroup(QString groupName) override;

    void refreshTrackInputSelection(int inputTrackIndex);

    void initialize() override;

    MainControllerStandalone * getMainController() override
    {
        return controller;
    }

protected:
    void closeEvent(QCloseEvent *);

    NinjamRoomWindow *createNinjamWindow(Login::RoomInfo, MainController *) override;

    void showPreferencesDialog(int initialTab) override;

    LocalTrackGroupViewStandalone *createLocalTrackGroupView(int channelGroupIndex) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, Persistence::Subchannel subChannel) override;

    void restoreLocalSubchannelPluginsList(LocalTrackViewStandalone *subChannelView, Persistence::Subchannel subChannel);

protected slots: //TODO change to private slots?
    void handleServerConnectionError(QString msg);

    void setGlobalPreferences(QList<bool>, int audioDevice, int firstIn, int lastIn, int firstOut,
                              int lastOut, int sampleRate, int bufferSize);

    // plugin finder
    void showPluginScanDialog();
    void hidePluginScanDialog(bool finishedWithoutError);
    void addFoundedPlugin(QString name, QString group, QString path);
    void setCurrentScanningPlugin(QString pluginPath);
    void addPluginToBlackList(QString pluginPath);

private slots:
    void toggleFullScreen();

    void closePluginScanDialog();

private:
    MainControllerStandalone *controller;
    QScopedPointer<PluginScanDialog> pluginScanDialog;

    LocalTrackGroupViewStandalone *geTrackGroupViewByName(QString trackGroupName) const;

    bool midiDeviceIsValid(int deviceIndex) const;

    void sanitizeSubchannelInputSelections(LocalTrackView *subChannelView, Persistence::Subchannel subChannel);

    bool fullScreenViewMode;

    void setFullScreenStatus(bool fullScreen);

    void setupSignals();

    void setupShortcuts();

    void restoreWindowPosition();

    void initializePluginFinder();

};

#endif // MAINFRAMEVST_H
