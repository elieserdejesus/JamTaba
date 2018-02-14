#include "MainWindowPlugin.h"
#include "NinjamRoomWindowPlugin.h"
#include "MainControllerPlugin.h"
#include "PreferencesDialogPlugin.h"
#include "gui/LocalTrackView.h"
#include "gui/LocalTrackGroupView.h"
#include "audio/core/LocalInputNode.h"
#include "TopLevelTextEditorModifier.h"

#include <QDesktopWidget>

const QSize MainWindowPlugin::PLUGIN_WINDOW_MIN_SIZE = QSize(990, 600);

const quint32 MainWindowPlugin::ZOOM_STEP = 70;

MainWindowPlugin::MainWindowPlugin(MainControllerPlugin *mainController) :
    MainWindow(mainController),
    firstChannelIsInitialized(false),
    increaseWindowWidthAction(nullptr),
    decreaseWindowWidthAction(nullptr),
    increaseWindowHeightAction(nullptr),
    decreaseWindowHeightAction(nullptr)
{

    setWindowIcon(QIcon(":/images/logo.png"));

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

TextEditorModifier *MainWindowPlugin::createTextEditorModifier()
{
    return new TopLevelTextEditorModifier();
}

void MainWindowPlugin::updateLocalInputChannelsGeometry()
{
    MainWindow::updateLocalInputChannelsGeometry();

    bool usingMinimumSize = !canDecreaseWindowWidth();
    int localChannels = localGroupChannels.size();
    for (LocalTrackGroupView *localChannel : localGroupChannels) {
        if (usingMinimumSize && localChannels > 1)
            localChannel->setToNarrow();
        else
            localChannel->setToWide();
    }
}

void MainWindowPlugin::initializeWindowSizeMenu()
{
    QMenu *windowSizeMenu = new QMenu(tr("Window Size"));

    QMenu *windowWidthMenu = new QMenu(tr("Width"));
    increaseWindowWidthAction = windowWidthMenu->addAction(tr("Increase"), this, SLOT(increaseWidth()));
    decreaseWindowWidthAction = windowWidthMenu->addAction(tr("Decrease"), this, SLOT(decreaseWidth()));

    QMenu *windowHeightMenu = new QMenu(tr("Height"));
    increaseWindowHeightAction = windowHeightMenu->addAction(tr("Increase"), this, SLOT(increaseHeight()));
    decreaseWindowHeightAction = windowHeightMenu->addAction(tr("Decrease"), this, SLOT(decreaseHeight()));

    connect(windowSizeMenu, &QMenu::aboutToShow, this, &MainWindowPlugin::updateWindowSizeMenu);

    ui.menuView->addSeparator();
    ui.menuView->addMenu(windowSizeMenu);

    windowSizeMenu->addMenu(windowWidthMenu);
    windowSizeMenu->addMenu(windowHeightMenu);
}

void MainWindowPlugin::updateWindowSizeMenu()
{
    if (!increaseWindowWidthAction || !decreaseWindowWidthAction || !increaseWindowHeightAction || !decreaseWindowHeightAction)
        return;

    increaseWindowWidthAction->setEnabled(canIncreaseWindowWidth());
    decreaseWindowWidthAction->setEnabled(canDecreaseWindowWidth());
    increaseWindowHeightAction->setEnabled(canIncreaseWindowHeight());
    decreaseWindowHeightAction->setEnabled(canDecreaseWindowHeight());
}

NinjamRoomWindow *MainWindowPlugin::createNinjamWindow(const login::RoomInfo &roomInfo, controller::MainController *mainController)
{
    return new NinjamRoomWindowPlugin(this, roomInfo, dynamic_cast<MainControllerPlugin*>(mainController));
}

void MainWindowPlugin::removeAllInputLocalTracks()
{
    MainWindow::removeAllInputLocalTracks();

    firstChannelIsInitialized = false; // prepare for the next local input tracks initialization (loading presets)
}

void MainWindowPlugin::initializeLocalSubChannel(LocalTrackView *subChannelView, const persistence::SubChannel &subChannel)
{
    // load channels names, gain, pan, boost, mute
    MainWindow::initializeLocalSubChannel(subChannelView, subChannel);

    // VST plugin always use stereo audio input. We need ensure this when loading presets.
    auto inputTrackNode = mainController->getInputTrack(subChannelView->getInputIndex());
    if (inputTrackNode) {
        int channelIndex = !firstChannelIsInitialized ?  0 : 1;
        int firstInputIndex = channelIndex * 2; // using inputs 0 & 1 for the 1st channel and inputs 2 & 3 for the 2nd channel. Vst allow up to 2 channels and no subchannels.
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
    QDesktopWidget desktop;
    QSize screenSize = desktop.availableGeometry().size();
    float scaleFactor = static_cast<float>(screenSize.width())/screenSize.height();

    const int widthStep = ZOOM_STEP * scaleFactor;
    const int heightStep = ZOOM_STEP;

    return QSize(widthStep, heightStep);
}

QSize MainWindowPlugin::getMaxWindowSize() const
{
    QDesktopWidget desktop;
    return desktop.availableGeometry().size();
}

bool MainWindowPlugin::canIncreaseWindowHeight() const
{
    QSize maxWindowsSize = getMaxWindowSize();
    return height() < maxWindowsSize.height();
}

bool MainWindowPlugin::canIncreaseWindowWidth() const
{
    QSize maxWindowsSize = getMaxWindowSize();
    return width() < maxWindowsSize.width();
}

bool MainWindowPlugin::canDecreaseWindowWidth() const
{
    QSize minWindowsSize = PLUGIN_WINDOW_MIN_SIZE;
    return width() > minWindowsSize.width();
}

bool MainWindowPlugin::canDecreaseWindowHeight() const
{
    QSize minWindowsSize = PLUGIN_WINDOW_MIN_SIZE;
    return height() > minWindowsSize.height();
}

void MainWindowPlugin::increaseHeight()
{
    if (isMaximized() || isFullScreen())
        return;

    if (!canIncreaseWindowHeight())
        return;

    QSize stepSize = getZoomStepSize();
    QSize maxSize = getMaxWindowSize();
    const int newWidth = width(); // changing just the height
    const int newHeight = qMin(height() + stepSize.height(), maxSize.height());

    resize(newWidth, newHeight);

    getMainController()->resizePluginEditor(newWidth, newHeight);
}

void MainWindowPlugin::increaseWidth()
{
    if (isMaximized() || isFullScreen())
        return;

    if (!canIncreaseWindowWidth())
        return;

    QSize stepSize = getZoomStepSize();
    QSize maxSize = getMaxWindowSize();
    const int newWidth = qMin(width() + stepSize.width(), maxSize.width());
    const int newHeight = height(); // changing just the width

    resize(newWidth, newHeight);

    getMainController()->resizePluginEditor(newWidth, newHeight);
}

void MainWindowPlugin::decreaseHeight()
{
    if (isMaximized() || isFullScreen())
        return;

    if (!canDecreaseWindowHeight())
        return;

    QSize stepSize = getZoomStepSize();
    QSize minSize = PLUGIN_WINDOW_MIN_SIZE;
    const int newWidth = width();
    const int newHeight = qMax(height() - stepSize.height(), minSize.height());

    resize(newWidth, newHeight);

    getMainController()->resizePluginEditor(newWidth, newHeight);
}

void MainWindowPlugin::decreaseWidth()
{
    if (isMaximized() || isFullScreen())
        return;

    if (!canDecreaseWindowWidth())
        return;

    QSize stepSize = getZoomStepSize();
    QSize minSize = PLUGIN_WINDOW_MIN_SIZE;
    const int newWidth = qMax(width() - stepSize.width(), minSize.width());
    const int newHeight = height(); // keep same height;

    resize(newWidth, newHeight);

    getMainController()->resizePluginEditor(newWidth, newHeight);
}


void MainWindowPlugin::resizeEvent(QResizeEvent *ev)
{
    MainWindow::resizeEvent(ev);

    updatePublicRoomsListLayout();
}
