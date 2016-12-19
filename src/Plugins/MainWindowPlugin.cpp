#include "MainWindowPlugin.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"
#include "PreferencesDialogPlugin.h"
#include "LocalTrackView.h"
#include "audio/core/LocalInputNode.h"

MainWindowVST::MainWindowVST(MainControllerPlugin *mainController) :
    MainWindow(mainController),
    firstChannelIsInitialized(false)
{
    this->ui.actionVstPreferences->setVisible(false);
    this->ui.actionAudioPreferences->setVisible(false);
    this->ui.actionMidiPreferences->setVisible(false);
    this->ui.actionQuit->setVisible(false);
    this->ui.actionFullscreenMode->setVisible(false);
}

NinjamRoomWindow *MainWindowVST::createNinjamWindow(const Login::RoomInfo &roomInfo,
                                                    Controller::MainController *mainController)
{
    return new NinjamRoomWindowPlugin(this, roomInfo, dynamic_cast<MainControllerPlugin*>(mainController));
}

void MainWindowVST::removeAllInputLocalTracks(){
    MainWindow::removeAllInputLocalTracks();

    firstChannelIsInitialized = false;//prepare for the next local input tracks initialization (loading presets)
}

void MainWindowVST::initializeLocalSubChannel(LocalTrackView *subChannelView, const Persistence::Subchannel &subChannel){

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

void MainWindowVST::setFullViewStatus(bool fullViewActivated)
{
    MainWindow::setFullViewStatus(fullViewActivated);
    MainControllerPlugin *controller = getMainController();
    controller->storeWindowSettings(isMaximized(), fullViewActivated, QPointF());
    controller->resizePluginEditor(width(), height());
}


PreferencesDialog *MainWindowVST::createPreferencesDialog()
{
    PreferencesDialog * dialog = new PreferencesDialogPlugin(this);
    setupPreferencesDialogSignals(dialog);
    return dialog;
}
