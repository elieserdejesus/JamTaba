#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "MainWindow.h"
#include "StandaloneLocalTrackGroupView.h"
#include "StandaloneLocalTrackView.h"
#include "StandAloneMainController.h"
#include "PluginScanDialog.h"

using namespace Controller;

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    MainWindowStandalone(StandaloneMainController *controller);

    Persistence::LocalInputTrackSettings getInputsSettings() const override;

    void addChannelsGroup(QString groupName) override;

    void refreshTrackInputSelection(int inputTrackIndex);

    void initialize() override;

protected:
    void closeEvent(QCloseEvent *);

    NinjamRoomWindow *createNinjamWindow(Login::RoomInfo, MainController *) override;

    void showPreferencesDialog(int initialTab) override;

    LocalTrackGroupView *createLocalTrackGroupView(int channelGroupIndex) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, Persistence::Subchannel subChannel) override;

    void restoreLocalSubchannelPluginsList(StandaloneLocalTrackView *subChannelView, Persistence::Subchannel subChannel);

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
    StandaloneMainController *controller;
    QScopedPointer<PluginScanDialog> pluginScanDialog;

    StandaloneLocalTrackGroupView *geTrackGroupViewByName(QString trackGroupName) const;

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
