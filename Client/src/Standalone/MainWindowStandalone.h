#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "MainWindow.h"
#include "StandaloneLocalTrackGroupView.h"
#include "StandaloneLocalTrackView.h"
#include "StandAloneMainController.h"

using namespace Controller;

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    MainWindowStandalone(StandaloneMainController *controller);

    Persistence::LocalInputTrackSettings getInputsSettings() const override;

    void addChannelsGroup(QString groupName) override;

    void refreshTrackInputSelection(int inputTrackIndex);

protected:
    void closeEvent(QCloseEvent *);

    NinjamRoomWindow *createNinjamWindow(Login::RoomInfo, MainController *) override;

    void initializePluginFinder() override;

    void showPreferencesDialog(int initialTab) override;

    LocalTrackGroupView *createLocalTrackGroupView(int channelGroupIndex) override;

    void initializeLocalSubChannel(LocalTrackView *subChannelView, Persistence::Subchannel subChannel) override;

    void restoreLocalSubchannelPluginsList(StandaloneLocalTrackView *subChannelView, Persistence::Subchannel subChannel);

protected slots:
    void handleServerConnectionError(QString msg);

    void setGlobalPreferences(QList<bool>, int audioDevice, int firstIn, int lastIn, int firstOut,
                              int lastOut, int sampleRate, int bufferSize);

private:
    StandaloneMainController *controller;

    StandaloneLocalTrackGroupView *geTrackGroupViewByName(QString trackGroupName) const;

    bool midiDeviceIsValid(int deviceIndex) const;

    void sanitizeSubchannelInputSelections(LocalTrackView *subChannelView, Persistence::Subchannel subChannel);
};

#endif // MAINFRAMEVST_H
