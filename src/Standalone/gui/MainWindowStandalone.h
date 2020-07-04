#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "gui/MainWindow.h"
#include "LocalTrackGroupViewStandalone.h" // necessary to return covariant type

namespace controller
{
    class MainControllerStandalone;
}

class LocalTrackView;
class LocalTrackViewStandalone;
class PluginScanDialog;

using controller::MainControllerStandalone;
using controller::MainController;
using persistence::SubChannel;

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    explicit MainWindowStandalone(MainControllerStandalone *controller);

    persistence::LocalInputTrackSettings getInputsSettings() const override;

    void addChannelsGroup(int instrumentIndex) override;

    void refreshTrackInputSelection(int inputTrackIndex);

    void initialize() override;

    MainControllerStandalone *getMainController() const override
    {
        return controller;
    }

protected:
    void closeEvent(QCloseEvent *) override;

    TextEditorModifier *createTextEditorModifier() override;

    NinjamRoomWindow *createNinjamWindow(const login::RoomInfo &, MainController *) override;

    LocalTrackGroupViewStandalone *createLocalTrackGroupView(int channelGroupIndex) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, const SubChannel &subChannel) override;

    void restoreLocalSubchannelPluginsList(LocalTrackViewStandalone *subChannelView, const SubChannel &subChannel);

    PreferencesDialog *createPreferencesDialog() override;

protected slots: // TODO change to private slots?

    void setGlobalPreferences(const QList<bool> &midiInputsStatus, const QList<bool> &syncOutputsStatus,
                              QString audioInputDevice, QString audioOutputDevice,
                              int firstIn, int lastIn, int firstOut, int lastOut);

    // plugin finder
    void showPluginScanDialog();
    void hidePluginScanDialog(bool finishedWithoutError);
    void showFoundedVstPlugin(const QString &name, const QString &path);
    void setCurrentScanningPlugin(const QString &pluginPath);
    void addPluginToBlackList(const QString &pluginPath);

    void doWindowInitialization() override;

private slots:
    void toggleFullScreen();
    void closePluginScanDialog();
    void restartAudioAndMidi();

    void tryClose(); // called when ESC is pressed

private:
    MainControllerStandalone *controller;
    PluginScanDialog *pluginScanDialog;

    PreferencesDialog *preferencesDialog; // store the instance to check if dialog is visible e decide show or not the Vst Plugin Scan Dialog

    LocalTrackGroupViewStandalone *geTrackGroupViewByName(const QString &trackGroupName) const;

    bool midiDeviceIsValid(int deviceIndex) const;

    void sanitizeSubchannelInputSelections(LocalTrackView *subChannelView, const persistence::SubChannel &subChannel);

    bool fullScreenViewMode;

    void setFullScreenStatus(bool fullScreen);

    void setupSignals();

    void setupShortcuts();

    void restoreWindowPosition();

    void initializePluginFinder();

    // standalone settings persistency management
    void readWindowSettings(bool isWindowMaximized);
    void writeWindowSettings();
};

#endif // MAINFRAMEVST_H
