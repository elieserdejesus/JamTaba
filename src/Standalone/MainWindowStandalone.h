#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "MainWindow.h"
#include "StandaloneLocalTrackGroupView.h"
#include "StandAloneMainController.h"

using namespace Controller;

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    MainWindowStandalone(StandaloneMainController *controller);

    void loadPresetToTrack() override;

    Persistence::InputsSettings getInputsSettings() const override;

    void addChannelsGroup(QString groupName) override;

    void refreshTrackInputSelection(int inputTrackIndex);

protected:
    void showEvent(QShowEvent *ent) override;

    void closeEvent(QCloseEvent *);

    NinjamRoomWindow *createNinjamWindow(Login::RoomInfo, MainController *) override;

    void initializePluginFinder() override;

    void showPreferencesDialog(int initialTab) override;

    void loadSubChannel(Persistence::Subchannel subChannel, LocalTrackView* subChannelView) override;
    void initializeSubChannel(Persistence::Subchannel subChannel, LocalTrackView* subChannelView) override;

protected slots:
    void handleServerConnectionError(QString msg);

    void setGlobalPreferences(QList<bool>, int audioDevice, int firstIn, int lastIn, int firstOut,
                              int lastOut, int sampleRate, int bufferSize);

private:

    StandaloneMainController* controller;

    void restorePluginsList();

    StandaloneLocalTrackGroupView* geTrackGroupViewByName(QString trackGroupName) const;


};

#endif // MAINFRAMEVST_H
