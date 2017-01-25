#include "MainWindowPlugin.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"
#include "PreferencesDialogPlugin.h"
#include "LocalTrackView.h"
#include "audio/core/LocalInputNode.h"

#include <QDesktopWidget>

const QSize MainWindowPlugin::PLUGIN_WINDOW_MIN_SIZE = QSize(800, 600);

const quint32 MainWindowPlugin::ZOOM_STEP = 100;

MainWindowPlugin::MainWindowPlugin(MainControllerPlugin *mainController) :
    MainWindow(mainController),
    firstChannelIsInitialized(false)
{
    ui.actionVstPreferences->setVisible(false);
    ui.actionAudioPreferences->setVisible(false);
    ui.actionMidiPreferences->setVisible(false);
    ui.actionQuit->setVisible(false);
    ui.actionFullscreenMode->setVisible(false);

    initializeWindowSizeMenu();
    
#ifdef Q_OS_MAC
    ui.menuBar->setNativeMenuBar(false); // avoid show the JamTaba menu bar in top of screen (the common behavior for mac apps)
#endif
}

void MainWindowPlugin::initializeWindowSizeMenu()
{

    QMenu *windowSizeMenu = new QMenu(tr("Window Size"));
    windowSizeMenu->addAction(tr("Increase"), this, SLOT(zoomIn()));
    windowSizeMenu->addAction(tr("Decrease"), this, SLOT(zoomOut()));
    ui.menuView->addSeparator();
    ui.menuView->addMenu(windowSizeMenu);
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

PreferencesDialog *MainWindowPlugin::createPreferencesDialog()
{
    PreferencesDialog * dialog = new PreferencesDialogPlugin(this);
    setupPreferencesDialogSignals(dialog);
    return dialog;
}

void MainWindowPlugin::zoomIn()
{
    if (isMaximized() || isFullScreen())
        return;

    QDesktopWidget desktop;
    QSize screenSize = desktop.availableGeometry().size();

    float scaleFactor = (float)width()/height();

    int newWidth = width() + (ZOOM_STEP * scaleFactor);
    int newHeight = height() + ZOOM_STEP;
    QSize newSize(qMin(screenSize.width(), newWidth), qMin(screenSize.height(), newHeight));

    resize(newSize);

    getMainController()->resizePluginEditor(newSize.width(), newSize.height());
}

void MainWindowPlugin::resizeEvent(QResizeEvent *ev)
{
    MainWindow::resizeEvent(ev);

    updatePublicRoomsListLayout();
}

void MainWindowPlugin::zoomOut()
{
    if (isMaximized() || isFullScreen())
        return;

    float scaleFactor = (float)width()/height();

    int newWidth = width() - (ZOOM_STEP * scaleFactor);
    int newHeight = height() - ZOOM_STEP;
    QSize newSize(qMax(minimumSize().width(), newWidth), qMax(minimumSize().height(), newHeight));

    resize(newSize);

    getMainController()->resizePluginEditor(newSize.width(), newSize.height());
}

bool MainWindowPlugin::canUseTwoColumnLayout() const
{
    if (width() >= MAIN_WINDOW_MIN_SIZE.width())
        return true;

    return false;
}
