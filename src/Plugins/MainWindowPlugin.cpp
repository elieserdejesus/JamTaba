#include "MainWindowPlugin.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"
#include "PreferencesDialogPlugin.h"
#include "LocalTrackView.h"
#include "audio/core/LocalInputNode.h"

MainWindowPlugin::MainWindowPlugin(MainControllerPlugin *mainController) :
    MainWindow(mainController),
    firstChannelIsInitialized(false)
{
    ui.actionVstPreferences->setVisible(false);
    ui.actionAudioPreferences->setVisible(false);
    ui.actionMidiPreferences->setVisible(false);
    ui.actionQuit->setVisible(false);
    ui.actionFullscreenMode->setVisible(false);
    
#ifdef Q_OS_MAC
    ui.menuBar->setNativeMenuBar(false); // avoid show the JamTaba menu bar in top of screen (the common behavior for mac apps)
#endif
}

NinjamRoomWindow *MainWindowPlugin::createNinjamWindow(const Login::RoomInfo &roomInfo,
                                                    Controller::MainController *mainController)
{
    return new NinjamRoomWindowPlugin(this, roomInfo, dynamic_cast<MainControllerPlugin*>(mainController));
}

void MainWindowPlugin::removeAllInputLocalTracks(){
    MainWindow::removeAllInputLocalTracks();

    firstChannelIsInitialized = false;//prepare for the next local input tracks initialization (loading presets)
}

void MainWindowPlugin::initializeLocalSubChannel(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel){

    // load channels names, gain, pan, boost, mute
    MainWindow::initializeLocalSubChannel(subChannelView, subChannel);

    // VST plugin always use stereo audio input. We need ensure this when loading presets.
    Audio::LocalInputNode *inputTrackNode = mainController->getInputTrack(subChannelView->getInputIndex());
    if(inputTrackNode){
        int channelIndex = !firstChannelIsInitialized ?  0 : 1;
        int firstInputIndex = channelIndex * 2;//using inputs 0 & 1 for the 1st channel and inputs 2 & 3 for the 2nd channel. Vst allow up to 2 channels and no subchannels.
        inputTrackNode->setAudioInputSelection(firstInputIndex, 2);//stereo
        firstChannelIsInitialized = true;
    }
}

// ++++++++++++++++++++++++++++

void MainWindowPlugin::setFullViewStatus(bool fullViewActivated)
{
    MainWindow::setFullViewStatus(fullViewActivated);
    MainControllerPlugin *controller = getMainController();
    controller->storeWindowSettings(isMaximized(), fullViewActivated, QPointF());
    controller->resizePluginEditor(width(), height());
}


PreferencesDialog *MainWindowPlugin::createPreferencesDialog()
{
    PreferencesDialog * dialog = new PreferencesDialogPlugin(this);
    setupPreferencesDialogSignals(dialog);
    return dialog;
}
