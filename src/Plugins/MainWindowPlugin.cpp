#include "MainWindowPlugin.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"
#include "PreferencesDialogPlugin.h"
#include "LocalTrackView.h"
#include "audio/core/LocalInputNode.h"
#include "TopLevelTextEditorModifier.h"

#include <QDesktopWidget>

const QSize MainWindowPlugin::PLUGIN_WINDOW_MIN_SIZE = QSize(990, 600);

const quint32 MainWindowPlugin::ZOOM_STEP = 100;


MainWindowPlugin::MainWindowPlugin(MainControllerPlugin *mainController) :
    MainWindow(mainController),
    firstChannelIsInitialized(false),
    increaseWindowSizeAction(nullptr),
    decreaseWindowSizeAction(nullptr)
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

void MainWindowPlugin::timerEvent(QTimerEvent *ev)
{
    MainWindow::timerEvent(ev);

    //check if mouse is outsize window and clear focus. This is necessary to avoid 'floating' QLineEdits when user drag AU/VSt window with some QLineEdit focused.
    if (QApplication::focusWidget()) {
        if (!rect().contains(mapFromGlobal(QCursor::pos())))
            QApplication::focusWidget()->clearFocus();
    }
}

TextEditorModifier *MainWindowPlugin::createTextEditorModifier()
{
    return new TopLevelTextEditorModifier();
}

void MainWindowPlugin::updateLocalInputChannelsGeometry()
{
    MainWindow::updateLocalInputChannelsGeometry();

    bool usingMinimumSize = !canDecreaseWindowSize();
    int localChannels = localGroupChannels.size();
    for(LocalTrackGroupView *localChannel : localGroupChannels) {
        if (usingMinimumSize && localChannels > 1)
            localChannel->setToNarrow();
        else
            localChannel->setToWide();
    }
}

void MainWindowPlugin::initializeWindowSizeMenu()
{

    QMenu *windowSizeMenu = new QMenu(tr("Window Size"));
    increaseWindowSizeAction = windowSizeMenu->addAction(tr("Increase"), this, SLOT(zoomIn()));
    decreaseWindowSizeAction = windowSizeMenu->addAction(tr("Decrease"), this, SLOT(zoomOut()));

    connect(windowSizeMenu, &QMenu::aboutToShow, this, &MainWindowPlugin::updateWindowSizeMenu);

    ui.menuView->addSeparator();
    ui.menuView->addMenu(windowSizeMenu);
}

void MainWindowPlugin::updateWindowSizeMenu()
{
    if (!increaseWindowSizeAction || !decreaseWindowSizeAction)
        return;

    increaseWindowSizeAction->setEnabled(canIncreaseWindowSize());
    decreaseWindowSizeAction->setEnabled(canDecreaseWindowSize());
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

QSize MainWindowPlugin::getZoomStepSize() const
{
    float scaleFactor = (float)width()/height();

    const int widthStep = ZOOM_STEP * scaleFactor;
    const int heightStep = ZOOM_STEP;

    return QSize(widthStep, heightStep);
}

QSize MainWindowPlugin::getMaxWindowSize() const
{
    QDesktopWidget desktop;
    QSize screenSize = desktop.availableGeometry().size();

    QSize stepSize = getZoomStepSize();

    const int maxWidth = screenSize.width() - stepSize.width();
    const int maxHeight = screenSize.height() - stepSize.height();

    return QSize(maxWidth, maxHeight);
}

bool MainWindowPlugin::canIncreaseWindowSize() const
{
    QSize stepSize = getZoomStepSize();
    QSize maxWindowsSize = getMaxWindowSize();

    const int newWidth = width() + stepSize.width();
    const int newHeight = height() + stepSize.height();
    const int maxWidth = maxWindowsSize.width();
    const int maxHeight= maxWindowsSize.height();

    return newWidth <= maxWidth && newHeight <= maxHeight;
}

bool MainWindowPlugin::canDecreaseWindowSize() const
{
    QSize stepSize = getZoomStepSize();
    QSize minWindowsSize = PLUGIN_WINDOW_MIN_SIZE;

    const int newWidth = width() - stepSize.width();
    const int newHeight = height() - stepSize.height();
    const int minWidth = minWindowsSize.width();
    const int minHeight= minWindowsSize.height();

    return newWidth >= minWidth && newHeight >= minHeight;
}

void MainWindowPlugin::zoomIn()
{
    if (isMaximized() || isFullScreen())
        return;

    if (!canIncreaseWindowSize())
        return;

    QSize stepSize = getZoomStepSize();
    const int newWidth = width() + stepSize.width();
    const int newHeight = height() + stepSize.height();

    resize(newWidth, newHeight);

    getMainController()->resizePluginEditor(newWidth, newHeight);

}

void MainWindowPlugin::zoomOut()
{
    if (isMaximized() || isFullScreen())
        return;

    if (!canDecreaseWindowSize())
        return;

    QSize stepSize = getZoomStepSize();
    const int newWidth = width() - stepSize.width();
    const int newHeight = height() - stepSize.height();

    resize(newWidth, newHeight);

    getMainController()->resizePluginEditor(newWidth, newHeight);
}

void MainWindowPlugin::resizeEvent(QResizeEvent *ev)
{
    MainWindow::resizeEvent(ev);

    updatePublicRoomsListLayout();
}
