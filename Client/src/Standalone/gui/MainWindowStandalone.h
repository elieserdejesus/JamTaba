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

    void addChannelsGroup(const QString &groupName) override;

    void refreshTrackInputSelection(int inputTrackIndex);

    void initialize() override;

    MainControllerStandalone * getMainController() override
    {
        return controller;
    }

protected:
    void closeEvent(QCloseEvent *);

    NinjamRoomWindow *createNinjamWindow(const Login::RoomInfo &, MainController *) override;

    LocalTrackGroupViewStandalone *createLocalTrackGroupView(int channelGroupIndex) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel) override;

    void restoreLocalSubchannelPluginsList(LocalTrackViewStandalone *subChannelView, const Persistence::Subchannel &subChannel);

    PreferencesDialog *createPreferencesDialog() override;

protected slots: //TODO change to private slots?
    void handleServerConnectionError(const QString &msg);

    void setGlobalPreferences(const QList<bool> &, int audioDevice, int firstIn, int lastIn, int firstOut,
                              int lastOut);

    // plugin finder
    void showPluginScanDialog();
    void hidePluginScanDialog(bool finishedWithoutError);
    void addFoundedPlugin(const QString &name, const QString &group, const QString &path);
    void setCurrentScanningPlugin(const QString &pluginPath);
    void addPluginToBlackList(const QString &pluginPath);

private slots:
    void toggleFullScreen();

    void closePluginScanDialog();

    void restartAudioAndMidi();

private:
    MainControllerStandalone *controller;
    QScopedPointer<PluginScanDialog> pluginScanDialog;

    LocalTrackGroupViewStandalone *geTrackGroupViewByName(const QString &trackGroupName) const;

    bool midiDeviceIsValid(int deviceIndex) const;

    void sanitizeSubchannelInputSelections(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel);

    bool fullScreenViewMode;

    void setFullScreenStatus(bool fullScreen);

    void setupSignals();

    void setupShortcuts();

    void restoreWindowPosition();

    void initializePluginFinder();

};

#endif // MAINFRAMEVST_H
