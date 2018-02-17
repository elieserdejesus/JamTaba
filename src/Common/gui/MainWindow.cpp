#include "MainWindow.h"
#include "LooperWindow.h"
#include "LocalTrackView.h"
#include "LocalTrackGroupView.h"
#include "NinjamRoomWindow.h"
#include "UserNameDialog.h"
#include "JamRoomViewPanel.h"
#include "widgets/MapWidget.h"
#include "widgets/ChatTabWidget.h"
#include "PreferencesDialog.h"
#include "PrivateServerDialog.h"
#include "PrivateServerWindow.h"
#include "chords/ChordsPanel.h"
#include "chords/ChatChordsProgressionParser.h"
#include "widgets/BlinkableButton.h"
#include "InactivityDetector.h"
#include "IconFactory.h"
#include "ThemeLoader.h"
#include "Highligther.h"
#include "NinjamController.h"
#include "MainController.h"
#include "Utils.h" // TODO refactoring to AudioUtils.h and use namespace
#include "GuiUtils.h"
#include "BusyDialog.h"
#include "UsersColorsPool.h"
#include "screensaver/ScreensaverBlocker.h"
#include "log/Logging.h"
#include "audio/core/LocalInputNode.h"
#include "audio/RoomStreamerNode.h"
#include "performance/PerformanceMonitor.h"
#include "video/VideoFrameGrabber.h"
#include "chat/NinjamVotingMessageParser.h"

#include <QDesktopWidget>
#include <QDesktopServices>
#include <QRect>
#include <QDateTime>
#include <QImage>
#include <QCameraInfo>

const QSize MainWindow::MAIN_WINDOW_MIN_SIZE = QSize(1100, 665);
const QString MainWindow::NIGHT_MODE_SUFFIX = "_nm";

const quint8 MainWindow::DEFAULT_REFRESH_RATE = 30; // in Hertz
const quint8 MainWindow::MAX_REFRESH_RATE = 60; // in Hertz

const quint32 MainWindow::DEFAULT_NETWORK_USAGE_UPDATE_PERIOD = 4000; // 4 seconds

const int MainWindow::PERFORMANCE_MONITOR_REFRESH_TIME = 200; //in miliseconds

const QString MainWindow::JAMTABA_CHAT_BOT_NAME("JamTaba");

using persistence::LocalInputTrackSettings;
using persistence::Channel;
using persistence::SubChannel;
using persistence::Preset;
using login::LoginService;
using controller::MainController;
using controller::NinjamController;
using audio::ChannelRange;
using audio::AbstractMp3Streamer;

MainWindow::MainWindow(MainController *mainController, QWidget *parent) :
    QMainWindow(parent),
    mainController(mainController),
    camera(nullptr),
    videoFrameGrabber(nullptr),
    cameraView(nullptr),
    cameraCombo(nullptr),
    cameraLayout(nullptr),
    bottomCollapsed(false),
    busyDialog(new BusyDialog()),
    bpmVotingExpirationTimer(nullptr),
    bpiVotingExpiratonTimer(nullptr),
    buttonCollapseLocalChannels(nullptr),
    buttonCollapseChat(nullptr),
    buttonCollapseBottomArea(nullptr),
    performanceMonitorLabel(nullptr),
    xmitInactivityDetector(nullptr),
    screensaverBlocker(new ScreensaverBlocker()),
    usersColorsPool(new UsersColorsPool()),
    chatTabWidget(nullptr),
    ninjamWindow(nullptr),
    roomToJump(nullptr),
    chordsPanel(nullptr),
    performanceMonitor(new PerformanceMonitor()),
    lastPerformanceMonitorUpdate(0)
{
    qCDebug(jtGUI) << "Creating MainWindow...";

    ui.setupUi(this);

    setWindowTitle("JamTaba");

    initializeLoginService();
    initializeMainTabWidget();
    setupMainTabCornerWidgets();
    initializeCollapseButtons();
    initializeViewMenu();
    initializeMasterFader();
    initializeLanguageMenu();
    initializeTranslator();
    initializeThemeMenu();
    initializeMeteringOptions();
    initializeCameraWidget();
    setupWidgets();
    setupSignals();

    setNetworkUsageUpdatePeriod(MainWindow::DEFAULT_NETWORK_USAGE_UPDATE_PERIOD);

    ChatPanel::setFontSizeOffset(mainController->getChatFontSizeOffset());

    qCDebug(jtGUI) << "MainWindow created!";

}

void MainWindow::setNetworkUsageUpdatePeriod(quint32 periodInMilliseconds)
{
    networkUsageUpdatePeriod = periodInMilliseconds;

    NinjamTrackView::setNetworkUsageUpdatePeriod(periodInMilliseconds);
}

void MainWindow::setupMainTabCornerWidgets()
{
    QWidget *frame = new QWidget();
    frame->setObjectName(QStringLiteral("right-corner"));
    QHBoxLayout *frameLayout = new QHBoxLayout();
    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(2);
    frame->setLayout(frameLayout);

    buttonCollapseBottomArea = new QPushButton(QIcon(":/images/collapse_bottom.png"), "");
    buttonCollapseChat = new QPushButton(QIcon(":/images/collapse_right.png"), "");
    buttonCollapseLocalChannels = new QPushButton(QIcon(":/images/collapse_left.png"), "");

    buttonCollapseBottomArea->setCheckable(true);
    buttonCollapseChat->setCheckable(true);
    buttonCollapseLocalChannels->setCheckable(true);

    buttonCollapseBottomArea->setObjectName(QStringLiteral("buttonCollapseBottom"));
    buttonCollapseChat->setObjectName(QStringLiteral("buttonCollapseChat"));
    buttonCollapseLocalChannels->setObjectName(QStringLiteral("buttonCollapseLocalTracks"));

    performanceMonitorLabel = new QLabel();
    performanceMonitorLabel->setObjectName(QStringLiteral("labelPerformanceMonitor"));

#ifndef Q_OS_WIN
   performanceMonitorLabel->setVisible(false); // showing RAM monitor in windows only
#endif

    transmitTransferRateLabel = new QLabel(this);
    transmitTransferRateLabel->setObjectName(QStringLiteral("transmitTransferRateLabel"));
    transmitTransferRateLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    transmitTransferRateLabel->setAlignment(Qt::AlignCenter);

    transmitIcon = new QLabel(this);
    transmitIcon->setObjectName("transmitIcon");
    transmitIcon->setPixmap(IconFactory::createTransmitPixmap(tintColor));

    receiveTransferRateLabel = new QLabel(this);
    receiveTransferRateLabel->setObjectName(QStringLiteral("receiveTransferRateLabel"));
    receiveTransferRateLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    receiveTransferRateLabel->setAlignment(Qt::AlignCenter);

    receiveIcon = new QLabel(this);
    receiveIcon->setObjectName("receiveIcon");
    receiveIcon->setPixmap(IconFactory::createReceivePixmap(tintColor));

    auto transferRateLayout = new QHBoxLayout();
    transferRateLayout->setContentsMargins(0, 0, 0, 0);
    transferRateLayout->setSpacing(0);
    transferRateLayout->addWidget(transmitIcon);
    transferRateLayout->addWidget(transmitTransferRateLabel, 1);
    transferRateLayout->addWidget(receiveIcon);
    transferRateLayout->addWidget(receiveTransferRateLabel, 1);

    frameLayout->addLayout(transferRateLayout);
    frameLayout->addSpacing(6);
    frameLayout->addWidget(performanceMonitorLabel);
    frameLayout->addSpacing(6);
    frameLayout->addWidget(buttonCollapseLocalChannels);
    frameLayout->addWidget(buttonCollapseBottomArea);
    frameLayout->addWidget(buttonCollapseChat);

    ui.contentTabWidget->setCornerWidget(frame);
}

void MainWindow::setCameraComboVisibility(bool show)
{
    if (!cameraCombo)
        return;

    if (show) {
        cameraCombo->clear();
        auto cameras = QCameraInfo::availableCameras();
        for (auto cameraInfo : cameras) {
            cameraCombo->addItem(cameraInfo.description(), cameraInfo.deviceName());
        }
    }
    else {
        cameraCombo->clear();
    }

    cameraCombo->setVisible(show && cameraCombo->count() > 1);
}

void MainWindow::initializeCamera(const QString &cameraDeviceName)
{
    if (camera) {
        camera->unload();
        camera->deleteLater();
    }

    preferredCameraName = cameraDeviceName.isEmpty() ? QCameraInfo::defaultCamera().deviceName() : cameraDeviceName;

    camera = new QCamera(preferredCameraName.toUtf8());

    connect(camera, static_cast<void(QCamera::*)(QCamera::Error)>(&QCamera::error), [=]()
    {
        QMessageBox::critical(this, tr("Error!"), camera->errorString());
    });

    if (videoFrameGrabber)
        camera->setViewfinder(videoFrameGrabber);

    camera->start();

    // setting resolution after start to fix #944 - https://github.com/elieserdejesus/JamTaba/issues/944
    QList<QSize> resolutions = camera->supportedViewfinderResolutions();
    if (!resolutions.isEmpty()) {
        QCameraViewfinderSettings settings;
        QSize lowResolution = resolutions.first();
        settings.setResolution(lowResolution);
        camera->setViewfinderSettings(settings);

        //getBestSupportedFrameRate();

        mainController->setVideoProperties(lowResolution);
    }
    else {
        qCritical() << "Camera resolutions list is empty!";
    }

    setCameraComboVisibility(camera->state() == QCamera::ActiveState);

    // adjust selected camera in combo
    for(int i = 0 ; i < cameraCombo->count(); i++) {
        if (cameraCombo->itemData(i) == preferredCameraName) {
            cameraCombo->setCurrentIndex(i);
            break;
        }
    }
}

void MainWindow::changeCameraStatus(bool activated)
{
    if (camera) {

        // camera is used by another application?
        if (camera->status() == QCamera::UnloadedStatus && camera->state() == QCamera::UnloadedState) {
            camera->unload();
            cameraView->setVisible(false);
            return;
        }
    }

    if (!activated) {
        if (camera) {
            camera->unload();
            camera->deleteLater();
            camera = nullptr;

            videoFrameGrabber->deleteLater();
            videoFrameGrabber = nullptr;

            setCameraComboVisibility(false);

            return;
        }
    }

    // activating the camera?

    if (!videoFrameGrabber) {
        videoFrameGrabber = new CameraFrameGrabber(this);

        connect(videoFrameGrabber, &CameraFrameGrabber::frameAvailable, [=](const QImage &frame) {

            if (mainController && !mainController->isPlayingInNinjamRoom()) {
                if (cameraView)
                    cameraView->setCurrentFrame(frame);
            }

        });
    }

    initializeCamera(preferredCameraName);

    if(camera && camera->state() != QCamera::ActiveState) { // camera is used by another application?
        camera->unload();

        videoFrameGrabber->deleteLater();
        videoFrameGrabber = nullptr;

        cameraView->activate(false);
    }

}

void MainWindow::selectNewCamera(int cameraIndex)
{
    QString cameraDeviceName = cameraCombo->itemData(cameraIndex).toString();
    initializeCamera(cameraDeviceName);
}

void MainWindow::initializeCameraWidget()
{
    auto cameras = QCameraInfo::availableCameras();

    if (!cameras.isEmpty()) {

        if (!cameraView) {
            QIcon webcamIcon = IconFactory::createWebcamIcon(tintColor);
            cameraView = new VideoWidget(this, webcamIcon, false);
            cameraView->setMaximumHeight(90);

            connect(cameraView, &VideoWidget::statusChanged, this, &MainWindow::changeCameraStatus);
        }

        if (!cameraLayout) {
            cameraLayout = new QVBoxLayout();

            cameraCombo = new QComboBox();
            cameraCombo->setObjectName(QStringLiteral("cameraCombo"));
            connect(cameraCombo, SIGNAL(activated(int)), this, SLOT(selectNewCamera(int)));
            cameraCombo->setVisible(false);

            cameraLayout->addWidget(cameraView, 0, Qt::AlignCenter);
            cameraLayout->addWidget(cameraCombo);

            QVBoxLayout *leftPanelLayout = static_cast<QVBoxLayout *>(ui.leftPanel->layout());
            leftPanelLayout->addLayout(cameraLayout);
        }
    }
}

QCamera::FrameRateRange MainWindow::getBestSupportedFrameRate() const
{
    return QCamera::FrameRateRange();
}

bool MainWindow::cameraIsActivated() const
{
    return cameraView && cameraView->isActivated();
}

QImage MainWindow::pickCameraFrame() const
{
    if (videoFrameGrabber && cameraView) {
        QImage frame = videoFrameGrabber->grab(cameraView->size());
        cameraView->setCurrentFrame(frame);
        return frame;
    }

    return QImage();
}

void MainWindow::initializeMeteringOptions()
{
    const auto &settings = mainController->getSettings();
    AudioMeter::setPaintMaxPeakMarker(settings.isShowingMaxPeaks());
    quint8 meterOption = settings.getMeterOption();
    switch (meterOption) {
    case 0:
        AudioMeter::paintPeaksAndRms();
        break;
    case 1:
        AudioMeter::paintPeaksOnly();
        break;
    case 2:
        AudioMeter::paintRmsOnly();
        break;
    }
}

void MainWindow::initializeTranslator()
{
    // always install the translators (one for Jamtaba strings and another for Qt strings)
    qApp->installTranslator(&jamtabaTranslator);
    qApp->installTranslator(&qtTranslator);

    // remember the last selected language
    QString localeName = mainController->getSettings().getTranslation();
    loadTranslationFile(localeName);
}

void MainWindow::setLanguage(QAction *languageMenuAction)
{
    QString locale = languageMenuAction->data().toString();
    loadTranslationFile(locale);
    mainController->setTranslationLanguage(locale);
    updatePublicRoomsListLayout();
    if (mainController->isPlayingInNinjamRoom()) {
        if (chatTabWidget) {
            chatTabWidget->setPreferredTranslationLanguage(locale);
        }

        ninjamWindow->updateGeoLocations();
    }
}

void MainWindow::handleThemeChanged()
{
    QString themeName = mainController->getTheme();
    MapWidget::setNightMode(MainWindow::themeCanUseNightModeWorldMaps(themeName));

    ui.masterMeter->updateStyleSheet();

    for (auto groupChannels : localGroupChannels) {
        for (auto trackView : groupChannels->getTracks<LocalTrackView *>()) {
            trackView->updateStyleSheet();
        }
    }


}

void MainWindow::setTheme(const QString &themeName)
{
    if (!mainController->setTheme(themeName)) {
        QString errorMessage = tr("Error loading the theme %1").arg(themeName);
        QMessageBox::critical(this, tr("Error!"), errorMessage);
    }
}

void MainWindow::changeTheme(QAction *action)
{
    QString themeName = action->data().toString();
    setTheme(themeName);
}

bool MainWindow::themeCanUseNightModeWorldMaps(const QString &themeName)
{
    return themeName.endsWith(NIGHT_MODE_SUFFIX);
}

QString MainWindow::getStripedThemeName(const QString &fullThemeName)
{
    if (themeCanUseNightModeWorldMaps(fullThemeName))
        return fullThemeName.left(fullThemeName.size() - 3); // remove the last 3 letters in theme name

    return fullThemeName;
}

void MainWindow::setTintColor(const QColor &color)
{
    this->tintColor = color;

    ui.localControlsCollapseButton->setIcon(IconFactory::createLocalControlsIcon(color));

    for(auto group : this->localGroupChannels) {
        group->setTintColor(color);
    }

    if (cameraView)
        cameraView->setIcon(IconFactory::createWebcamIcon(color));

    if (ninjamWindow)
        ninjamWindow->setTintColor(color);


    for (auto looperWindow : looperWindows)
        looperWindow->setTintColor(color);

    // master
    ui.speakerIconLeft->setPixmap(IconFactory::createLowLevelIcon(color));
    ui.speakerIconRight->setPixmap(IconFactory::createHighLevelIcon(color));

    if (chatTabWidget)
        chatTabWidget->setChatsTintColor(color);

    // network usage icons
    transmitIcon->setPixmap(IconFactory::createTransmitPixmap(color));
    receiveIcon->setPixmap(IconFactory::createReceivePixmap(color));
}

void MainWindow::initializeThemeMenu()
{
    connect(ui.menuTheme, &QMenu::triggered, this, &MainWindow::changeTheme);
    connect(ui.menuTheme, &QMenu::aboutToShow, this, &MainWindow::translateThemeMenu); // the menu is translated before open

    // create a menu action for each theme
    QString themesDir = Configurator::getInstance()->getThemesDir().absolutePath();
    auto themes = theme::Loader::getAvailableThemes(themesDir);
    for (const auto &themeDir : themes) {
        QString themeName = QFileInfo(themeDir).baseName();
        QAction *action = ui.menuTheme->addAction(getStripedThemeName(themeName));
        action->setData(themeName);
    }
}

void MainWindow::translateThemeMenu()
{
    for (QAction *action : ui.menuTheme->actions()) {
        QString themeName = getStripedThemeName(action->data().toString()); // use themeName as default text
        QString translatedName = getTranslatedThemeName(themeName);

        if (translatedName != themeName) // maybe we don't have a translation entry for the theme ...
            themeName = translatedName + " (" + themeName + ")";

        action->setText(themeName);
    }
}

QString MainWindow::getTranslatedThemeName(const QString &themeName)
{
    QMap<QString, QString> translatedNames;
    translatedNames.insert("Black",   tr("Black"));
    translatedNames.insert("Flat",    tr("Flat"));
    translatedNames.insert("Rounded", tr("Rounded"));
    translatedNames.insert("Volcano", tr("Volcano"));
    translatedNames.insert("Ice",     tr("Ice"));
    translatedNames.insert("Game",    tr("Game"));
    translatedNames.insert("Navy",    tr("Navy"));

    if (translatedNames.contains(themeName))
        return translatedNames[themeName];

    return themeName; // returning the original name if we have not a translate entry in the map
}

void MainWindow::loadTranslationFile(const QString &locale)
{
    if( !jamtabaTranslator.load(locale, ":/tr"))
        qWarning() << "Can't load the Jamtaba translation for " << locale;

    if (locale != "en") { // avoid load en Qt translation (this file is obviously missing)
        QString fileName = "qtbase_" + locale;
        if (!qtTranslator.load(fileName, ":/qt_tr"))
            qWarning() << "Can't load Qt translation for " << locale;
    }
}

void MainWindow::initializeLanguageMenu()
{
    // create a menu action for each translation resource
    QDir translationsDir(":/tr");
    if (translationsDir.exists()) {
        QStringList locales = translationsDir.entryList();
        for (auto translationFile : locales) {
            QLocale loc(translationFile);
            QString nativeLanguageName = gui::capitalize(loc.nativeLanguageName());
            QString englishLanguageName = gui::capitalize(QLocale::languageToString(loc.language())); // QLocale::languageToString is returning capitalized String, but just to be shure (Qt can change in future) I'm using capitalize here too.

            QString actionText = nativeLanguageName + " (" + englishLanguageName + ")";
            auto actions = ui.menuLanguage->actions();
            QAction *before = actions.isEmpty() ? nullptr : actions.first();
            while (before && before->text() < actionText) { // sorting the language menu entries
                actions.removeFirst();
                before = actions.isEmpty() ? nullptr : actions.first();
            }
            QAction *action = new QAction(actionText, ui.menuLanguage);
            action->setData(translationFile); // using the locale (pt_BR, ja_JP) as data
            ui.menuLanguage->insertAction(before, action);
        }
    } else {
        qCritical() << "translations dir not exist! Can't create the Language menu!";
    }
}

void MainWindow::initializeGuiRefreshTimer()
{
    quint8 refreshRate = mainController->getSettings().getMeterRefreshRate();
    if (refreshRate <= 0)
        refreshRate = DEFAULT_REFRESH_RATE;
    else if (refreshRate > MAX_REFRESH_RATE)
        refreshRate = MAX_REFRESH_RATE;

    timerID = startTimer(1000/refreshRate); // timer used to animate audio peaks, midi activity, public room wave audio plot, etc.
}

void MainWindow::initialize()
{
    // initialize text modifier here to avoid a pure virtual method call in constructor
    TextEditorModifier *textEditorModifier = createTextEditorModifier();
    if (textEditorModifier) {
        bool finishEditorPressingReturnKey = true;
        textEditorModifier->modify(ui.userNameLineEdit, finishEditorPressingReturnKey);
    }

    initializeGuiRefreshTimer();

    if (qApp->styleSheet().isEmpty()) { // allow custom stylesheet via app arguments
        QString themeName = mainController->getTheme();
        QString themesDir = Configurator::getInstance()->getThemesDir().absolutePath();
        if(!theme::Loader::canLoad(themesDir, themeName))
            themeName = "Flat"; // fallback to Flat theme
        setTheme(themeName);
    }

    showBusyDialog(tr("Loading rooms list ..."));

    doWindowInitialization();

    auto localChannels = getLocalChannels<LocalTrackGroupView *>();
    Q_ASSERT(!localChannels.isEmpty());
    auto firstChannelXmitButton = localChannels.first()->getXmitButton();
    uint intervalsBeforeInactivityWarning = mainController->getSettings().getIntervalsBeforeInactivityWarning();
    xmitInactivityDetector = new InactivityDetector(this, firstChannelXmitButton, intervalsBeforeInactivityWarning);

    // remember collapse status
    auto settings = mainController->getSettings();

    if (settings.isRememberingLocalChannels())
        showPeakMetersOnlyInLocalControls(settings.isLocalChannelsCollapsed());

    if (settings.isRememberingBottomSection())
        setBottomCollapsedStatus(settings.isBottomSectionCollapsed());

}

void MainWindow::doWindowInitialization()
{
    initializeLocalInputChannels(); // create the local tracks, load plugins, etc.
    initializeWindowSize();
}

void MainWindow::showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly)
{
    for (LocalTrackGroupView *channel : localGroupChannels) {
        channel->setPeakMeterMode(showPeakMetersOnly);
    }

    ui.localControlsCollapseButton->setChecked(showPeakMetersOnly);

    if (cameraView) {
        cameraView->setVisible(!showPeakMetersOnly);
        cameraCombo->setVisible(cameraView->isVisible() && cameraCombo->count() > 1);
    }

    updateLocalInputChannelsGeometry();

    ui.userNamePanel->setVisible(!showPeakMetersOnly);
    ui.labelYourControls->setVisible(!showPeakMetersOnly);

    mainController->setLocalChannelsCollapsed(showPeakMetersOnly);
}

void MainWindow::updateLocalInputChannelsGeometry()
{
    if (localGroupChannels.isEmpty())
        return;

    int minWidth = ui.localTracksWidget->sizeHint().width();
    minWidth += 12;
    int maxWidth = minWidth;

    ui.leftPanel->setMaximumWidth(maxWidth);
    ui.leftPanel->setMinimumWidth(minWidth);
}

void MainWindow::setBottomCollapsedStatus(bool collapsed)
{
    bottomCollapsed = collapsed;

    collapseBottomArea(collapsed);

    updateCollapseButtons();

    mainController->setBottomSectionCollapsed(collapsed); // store in settings
}

void MainWindow::toggleBottomAreaCollapseStatus()
{
    setBottomCollapsedStatus(!bottomCollapsed);
}

void MainWindow::toggleLocalTracksCollapseStatus()
{
    if (localGroupChannels.isEmpty())
        return;

    bool isShowingPeakMetersOnly = localGroupChannels.first()->isShowingPeakMeterOnly();
    showPeakMetersOnlyInLocalControls(!isShowingPeakMetersOnly); // toggle

    updateCollapseButtons();
}

persistence::LocalInputTrackSettings MainWindow::getInputsSettings() const
{
    LocalInputTrackSettings settings;
    for (auto trackGroupView : localGroupChannels) {
        Channel channel(trackGroupView->getGroupName());
        int subchannelsCount = 0;
        for (auto trackView : trackGroupView->getTracks<LocalTrackView *>()) {
            auto inputNode = trackView->getInputNode();
            ChannelRange inputNodeRange = inputNode->getAudioInputRange();
            int firstInput = inputNodeRange.getFirstChannel();
            int channels = inputNodeRange.getChannels();
            int midiDevice = inputNode->getMidiDeviceIndex();
            int midiChannel = inputNode->getMidiChannelIndex();
            float gain = Utils::poweredGainToLinear(inputNode->getGain());
            float boost = Utils::linearToDb(inputNode->getBoost());
            float pan = inputNode->getPan();
            bool muted = inputNode->isMuted();
            bool stereoInverted = inputNode->isStereoInverted();
            qint8 transpose = inputNode->getTranspose();
            quint8 lowerNote = inputNode->getMidiLowerNote();
            quint8 higherNote = inputNode->getMidiHigherNote();
            bool routindMidiInput = subchannelsCount > 0 && inputNode->isRoutingMidiInput(); // midi routing is not allowed in first subchannel

            SubChannel sub(firstInput, channels, midiDevice, midiChannel, gain, boost, pan, muted, stereoInverted,
                                                                    transpose, lowerNote, higherNote, routindMidiInput);
            channel.subChannels.append(sub);

            subchannelsCount++;
        }
        settings.channels.append(channel);
    }
    return settings;
}

void MainWindow::stopCurrentRoomStream()
{
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();

        if (roomViewPanels[roomID])
            roomViewPanels[roomID]->clear(true);

        mainController->stopRoomStream();
    }
}

void MainWindow::showMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon)
{
    QMessageBox *messageBox = new QMessageBox(nullptr);
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    messageBox->setIcon(icon);
    messageBox->setWindowIcon(this->windowIcon());
    messageBox->setAttribute(Qt::WA_DeleteOnClose, true);
    messageBox->show();
    messageBox->exec();
}

void MainWindow::handlePublicRoomStreamError(const QString &msg)
{
    stopCurrentRoomStream();
    showMessageBox(tr("Error!"), msg, QMessageBox::Critical);
}

void MainWindow::removeChannelsGroup(int channelIndex)
{
    if (localGroupChannels.size() > 1) { // the first channel group can't be removed
        if (channelIndex >= 0 && channelIndex < localGroupChannels.size()) {
            auto channel = localGroupChannels.at(channelIndex);
            ui.localTracksLayout->removeWidget(channel);
            localGroupChannels.removeAt(channelIndex);

            localGroupChannels.first()->setToWide();

            channel->deleteLater();

            // TODO Refactoring: emit a signal 'localChannel removed' and catch this signal in NinjamController
            mainController->sendRemovedChannelMessage(channelIndex);
            update();
        }
        updateLocalInputChannelsGeometry();
    }
}

void MainWindow::highlightChannelGroup(int index) const
{
    if (index >= 0 && index < localGroupChannels.size())
        Highligther::getInstance()->highlight(localGroupChannels.at(index));
}

void MainWindow::addChannelsGroup(const QString &name)
{
    int channelIndex = localGroupChannels.size();
    addLocalChannel(channelIndex, name, true);

    if (mainController->isPlayingInNinjamRoom()) {
        mainController->sendNewChannelsNames(getChannelsNames());

        // create an encoder for this channel in next interval
        mainController->getNinjamController()->scheduleEncoderChangeForChannel(channelIndex);
    }
    updateLocalInputChannelsGeometry();
}

void MainWindow::removeTabCloseButton(QTabWidget *tabWidget, int buttonIndex)
{
    if (!tabWidget)
        return;

    auto tabBar = tabWidget->tabBar()->tabButton(buttonIndex, QTabBar::RightSide);

    if (!tabBar) // try get the tabBar in left side (MAC OSX)
        tabBar = tabWidget->tabBar()->tabButton(buttonIndex, QTabBar::LeftSide);

    if (tabBar) {
        tabBar->resize(0, 0);
        tabBar->hide();
    }
}

void MainWindow::initializeMainTabWidget()
{
    // the rooms list tab bar is not closable

    removeTabCloseButton(ui.contentTabWidget, 0);

    connect(ui.contentTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeContentTab);
    connect(ui.contentTabWidget, &QTabWidget::tabBarClicked, this, &MainWindow::changeTab);
}

void MainWindow::updateChannelsNames()
{
    mainController->sendNewChannelsNames(getChannelsNames());
}

// This is a factory method and is overrided in derived classes to create more specific instances.
LocalTrackGroupView *MainWindow::createLocalTrackGroupView(int channelGroupIndex)
{
    return new LocalTrackGroupView(channelGroupIndex, this);
}

LocalTrackGroupView *MainWindow::addLocalChannel(int channelGroupIndex, const QString &channelName,
                                                 bool createFirstSubchannel)
{
    auto localChannel = createLocalTrackGroupView(channelGroupIndex);

    connect(localChannel, &LocalTrackGroupView::nameChanged, this, &MainWindow::updateChannelsNames);

    connect(localChannel, &LocalTrackGroupView::trackAdded, this, &MainWindow::updateLocalInputChannelsGeometry);

    connect(localChannel, &LocalTrackGroupView::trackRemoved, this, &MainWindow::updateLocalInputChannelsGeometry);

    if (!localGroupChannels.isEmpty()) // the second channel?
        localChannel->setPreparingStatus(localGroupChannels.at(0)->isPreparingToTransmit());

    localGroupChannels.append(localChannel);

    localChannel->setGroupName(channelName);
    ui.localTracksLayout->addWidget(localChannel);

    if (createFirstSubchannel) {
         localChannel->addTrackView(channelGroupIndex);
    }

    ui.localTracksWidget->updateGeometry();

    return localChannel;
}

void MainWindow::loadPreset(const Preset &preset)
{
    if (preset.isValid()) {
        removeAllInputLocalTracks();
        initializeLocalInputChannels(preset.inputTrackSettings);

        //set all loaded channels xmit to ON
        for (auto  trackGroupView :  localGroupChannels ) {
            mainController->setTransmitingStatus(trackGroupView->getChannelIndex(), true);
        }

        QApplication::processEvents();
    }
}

void MainWindow::removeAllInputLocalTracks()
{
    mainController->removeAllInputTracks();

    while (!localGroupChannels.isEmpty()) {
        auto view = localGroupChannels.first();
        ui.localTracksLayout->removeWidget(view);
        view->deleteLater();
        localGroupChannels.pop_front();
    }
}


// this function is overrided in MainWindowStandalone to load input selections and plugins
void MainWindow::initializeLocalSubChannel(LocalTrackView *localTrackView, const SubChannel &subChannel)
{
    auto boostValue = BaseTrackView::intToBoostValue(subChannel.boost);
    localTrackView->setInitialValues(subChannel.gain, boostValue, subChannel.pan, subChannel.muted, subChannel.stereoInverted);
}

void MainWindow::openLooperWindow(uint trackID)
{
    Q_ASSERT(mainController);

     auto inputTrack = mainController->getInputTrack(trackID);
     if (inputTrack) {
        auto looperWindow = looperWindows[trackID];
        if (!looperWindow) {
            looperWindow = new LooperWindow(this, mainController);
            looperWindows.insert(trackID, looperWindow);
            looperWindow->setTintColor(getTintColor());
        }

        looperWindow->setLooper(inputTrack->getLooper());

        auto channel = localGroupChannels.at(inputTrack->getChanneGrouplIndex());
        Q_ASSERT(channel);

        int subchannelInternalIndex = channel->getSubchannelInternalIndex(trackID);
        QString channelName = channel->getGroupName();
        if (channelName.isEmpty())
            channelName = tr("Channel %1").arg(QString::number(channel->getChannelIndex() + 1));
        else
            channelName = tr("Channel %1").arg(channelName);

        QString subchannelName("Subchannel " + QString::number(subchannelInternalIndex + 1));
        QString windowTitle("Looper - " + channelName + " (" + subchannelName + ")");
        looperWindow->setWindowTitle(windowTitle);

        looperWindow->raise();
        looperWindow->show();
        looperWindow->activateWindow();

     }
     else {
         qCritical() << "inputTrack or ninjamControler are null";
     }
}

void MainWindow::initializeLocalInputChannels()
{
    initializeLocalInputChannels(mainController->getSettings().getInputsSettings());
}

void MainWindow::initializeLocalInputChannels(const LocalInputTrackSettings &inputsSettings)
{
    QApplication::setOverrideCursor(Qt::WaitCursor); // this line was hanging/freezing in Mac

    int channelIndex = 0;
    for (const auto &channel : inputsSettings.channels) {
        qCDebug(jtGUI) << "\tCreating channel "<< channel.name;
        bool createFirstSubChannel = channel.subChannels.isEmpty();
        auto channelView = addLocalChannel(channelIndex, channel.name,
                                                           createFirstSubChannel);
        for (const auto &subChannel : channel.subChannels) {
            qCDebug(jtGUI) << "\t\tCreating sub-channel ";
            auto subChannelView = channelView->addTrackView(channelIndex);
            initializeLocalSubChannel(subChannelView, subChannel);
        }
        channelIndex++;
    }
    if (channelIndex == 0) // no channels in settings file or no settings file...
        addLocalChannel(0, "", true); // create a channel using an empty name

    qCDebug(jtGUI) << "Initializing local inputs done!";

    QApplication::restoreOverrideCursor();
}

void MainWindow::initializeLoginService()
{
    auto loginService = this->mainController->getLoginService();

    connect(loginService, &LoginService::roomsListAvailable, this, &MainWindow::refreshPublicRoomsList);

    connect(loginService, &LoginService::incompatibilityWithServerDetected, this, &MainWindow::handleIncompatiblity);

    connect(loginService, &LoginService::newVersionAvailableForDownload, this, &MainWindow::showNewVersionAvailableMessage);

    connect(loginService, &LoginService::errorWhenConnectingToServer, this, &MainWindow::handleServerConnectionError);
}

void MainWindow::closeContentTab(int index)
{
    if (index > 0) { // the first tab (rooms to play) is not closeable
        showBusyDialog(tr("disconnecting ..."));
        if (mainController->isPlayingInNinjamRoom())
            mainController->stopNinjamController();
    }
}

void MainWindow::changeTab(int index)
{
    if (index > 0) { // click in room tab?
        if (mainController->isPlayingInNinjamRoom() && mainController->isPlayingRoomStream())
            stopCurrentRoomStream();
    } else { // click in the public rooms tab?
        updatePublicRoomsListLayout();
    }
}

void MainWindow::showBusyDialog()
{
    showBusyDialog("");
}

void MainWindow::showBusyDialog(const QString &message)
{
    busyDialog->setParent(this);
    busyDialog->show(message);
    centerBusyDialog();

}

void MainWindow::hideBusyDialog()
{
    busyDialog->hide();
}

void MainWindow::centerBusyDialog()
{
    if (!busyDialog->parentWidget())
        return;

    QSize parentSize = busyDialog->parentWidget()->size();
    QSize busyDialogSize = busyDialog->size();
    int newX = parentSize.width()/2 - busyDialogSize.width()/2;
    int newY = parentSize.height()/2;
    busyDialog->move(newX, newY);
}

bool MainWindow::jamRoomLessThan(const login::RoomInfo &r1, const login::RoomInfo &r2)
{
    return r1.getNonBotUsersCount() > r2.getNonBotUsersCount();
}

void MainWindow::handleIncompatiblity()
{
    hideBusyDialog();
    QString text = tr("Your Jamtaba version is not compatible with previous versions!");
    QMessageBox::warning(this, tr("Server : Compatibility problem"), text);
    close();
}

void MainWindow::detachMainController()
{
    //if (videoFrameGrabber) { // necessary to avoid crash VST host when Jamtaba is removed
        //disconnect(videoFrameGrabber, &CameraFrameGrabber::frameAvailable, this, nullptr);
    //}

    if (camera) { // necessary to avoid crash VST host when Jamtaba is removed
        camera->unload();
    }

    mainController = nullptr;
}

void MainWindow::handleServerConnectionError(const QString &errorMsg)
{
    hideBusyDialog();
    QMessageBox::warning(this, tr("Error!"),
                         tr("Error connecting with Jamtaba server!\n") + errorMsg);
    close();
}

QString MainWindow::sanitizeLatestVersionDetails(const QString &details)
{
    QString newDetails(details);

    newDetails
        .replace(QChar('\r'), "")
        .replace(QChar('\n'), "<br>")
        .replace(QString("["), QString("[<b>"))
        .replace(QString("]"), QString("</b>]"));

    return newDetails;
}

void MainWindow::showNewVersionAvailableMessage(const QString &latestVersionDetails)
{
    hideBusyDialog();
    QString text = tr("A new Jamtaba version is available for download! Please use the <a href='http://www.jamtaba.com'>new version</a>!");

    if (!latestVersionDetails.isEmpty()) {
        text += "<br><br>";
        text += MainWindow::sanitizeLatestVersionDetails(latestVersionDetails);
    }

    QMessageBox::information(this, tr("New Jamtaba version available!"), text);
}

JamRoomViewPanel *MainWindow::createJamRoomViewPanel(const login::RoomInfo &roomInfo)
{
    auto newJamRoomView = new JamRoomViewPanel(roomInfo, mainController);

    connect(newJamRoomView, &JamRoomViewPanel::startingListeningTheRoom, this, &MainWindow::playPublicRoomStream);

    connect(newJamRoomView, &JamRoomViewPanel::finishingListeningTheRoom, this, &MainWindow::stopPublicRoomStream);

    connect(newJamRoomView, SIGNAL(enteringInTheRoom(login::RoomInfo)), this, SLOT(tryEnterInRoom(login::RoomInfo)));

    return newJamRoomView;
}

bool MainWindow::canUseTwoColumnLayoutInPublicRooms() const
{
    return ui.contentTabWidget->width() >= 860;
}

void MainWindow::refreshPublicRoomsList(const QList<login::RoomInfo> &publicRooms)
{
    if (publicRooms.isEmpty())
        return;

    hideBusyDialog();

    QList<login::RoomInfo> sortedRooms(publicRooms);
    qSort(sortedRooms.begin(), sortedRooms.end(), jamRoomLessThan);

    int index = 0;
    bool twoCollumns = canUseTwoColumnLayoutInPublicRooms();
    for (const auto &roomInfo : sortedRooms) {
        if (roomInfo.getType() == login::RoomTYPE::NINJAM) { // skipping other rooms at moment
            int rowIndex = twoCollumns ? (index / 2) : (index);
            int collumnIndex = twoCollumns ? (index % 2) : 0;
            auto roomViewPanel = roomViewPanels[roomInfo.getID()];
            if (roomViewPanel) {
                roomViewPanel->refresh(roomInfo);
                // check if is playing a public room stream but this room is empty now
                if (mainController->isPlayingRoomStream()) {
                    if (roomInfo.isEmpty() && mainController->getCurrentStreamingRoomID() == roomInfo.getID()) {
                        stopCurrentRoomStream();
                    }
                }
                ui.allRoomsContent->layout()->removeWidget(roomViewPanel); // the widget is removed but added again
            } else {
                roomViewPanel = createJamRoomViewPanel(roomInfo);
                roomViewPanels.insert(roomInfo.getID(), roomViewPanel);
            }
            auto layout = dynamic_cast<QGridLayout *>(ui.allRoomsContent->layout());
            layout->addWidget(roomViewPanel, rowIndex, collumnIndex);
            index++;
        }
    }

    if (mainController->isPlayingInNinjamRoom())
        this->ninjamWindow->updateGeoLocations();

    /** updating country flag and country names after refresh the public rooms list. This is necessary because the call to webservice used to get country codes and  country names is not synchronous. So, if country code and name are not cached we receive these data from the webservice after some seconds.*/
}

void MainWindow::playPublicRoomStream(const login::RoomInfo &roomInfo)
{
    // clear all plots
    for (auto viewPanel : this->roomViewPanels.values())
        viewPanel->clear(roomInfo.getID() != viewPanel->getRoomInfo().getID());

    if (roomInfo.hasStream()) // just in case...
        mainController->playRoomStream(roomInfo);
}

void MainWindow::stopPublicRoomStream(const login::RoomInfo &roomInfo)
{
    Q_UNUSED(roomInfo)
    stopCurrentRoomStream();
}

QStringList MainWindow::getChannelsNames() const
{
    QStringList channelsNames;
    for (LocalTrackGroupView *channel : localGroupChannels)
        channelsNames.append(channel->getGroupName());

    return channelsNames;
}

// user trying enter in a room
void MainWindow::tryEnterInRoom(const login::RoomInfo &roomInfo, const QString &password)
{
    // stop room stream before enter in a room
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();

        if (roomViewPanels[roomID])
            roomViewPanels[roomID]->clear(true);

        mainController->stopRoomStream();
    }

    // show the user name dialog
    if (!mainController->userNameWasChoosed()) {
        UserNameDialog dialog(ui.centralWidget, MainController::getSuggestedUserName());
        centerDialog(&dialog);
        if (dialog.exec() == QDialog::Accepted) {
            QString userName = dialog.getUserName().trimmed();
            if (!userName.isEmpty()) {
                mainController->setUserName(userName);
                ui.userNameLineEdit->setText(userName); // show the user name in top of local tracks
            } else {
                QMessageBox::warning(this, tr("Warning!"), tr("Empty name is not allowed!"));
            }
        } else {
            qWarning() << "name dialog canceled";
        }
    }

    if (mainController->isPlayingInNinjamRoom()) {
        // store the room to jump and wait for disconnectedFromServer event to connect in this new room
        roomToJump.reset(new login::RoomInfo(roomInfo));
        passwordToJump = password;

        mainController->stopNinjamController(); // disconnect from current ninjam server
    }
    else if (mainController->userNameWasChoosed()) {
        showBusyDialog(tr("Connecting with %1 ... ").arg(roomInfo.getName()));
        mainController->enterInRoom(roomInfo, getChannelsNames(), password);
    }
}

void MainWindow::enterInRoom(const login::RoomInfo &roomInfo)
{
    qCDebug(jtGUI) << "hidding busy dialog...";
    hideBusyDialog();

    setUserNameReadOnlyStatus(true); // lock the user name field, user name can't be changed when jamming

    qCDebug(jtGUI) << "creating NinjamRoomWindow...";
    ninjamWindow.reset(createNinjamWindow(roomInfo, mainController));
    QString tabName = roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")";
    int index = ui.contentTabWidget->addTab(ninjamWindow.data(), tabName);
    ui.contentTabWidget->setCurrentIndex(index);

    addMainChatPanel();

    Q_ASSERT(chatTabWidget);

    auto settings = mainController->getSettings();
    chatTabWidget->collapse(settings.isRememberingChatSection() && settings.isChatSectionCollapsed());

    addNinjamPanelsInBottom();

    ui.leftPanel->adjustSize();

    wireNinjamControllerSignals();

    enableLooperButtonInLocalTracks(true); // looper buttons are enabled when entering in a server

    qCDebug(jtGUI) << "MainWindow::enterInRoom() done!";
}

void MainWindow::collapseBottomArea(bool collapse)
{
    gui::setLayoutItemsVisibility(ui.masterControlsLayout, !collapse);

    bool canHandleNinjamPanels = mainController->isPlayingInNinjamRoom() && ninjamWindow && ninjamWindow->getNinjamPanel() && ninjamWindow->getMetronomePanel();

    if (canHandleNinjamPanels) {
        auto ninjamPanel = ninjamWindow->getNinjamPanel();
        ninjamPanel->setVisible(true);
        ninjamPanel->setCollapseMode(collapse);

        if (collapse)
            ui.bottomPanelLayout->addWidget(ninjamPanel, 1, 0, 1, 3); // re-add ninjamPanel using 3 cols for colspan
    }

    ui.masterTitleLabel->setVisible(canHandleNinjamPanels);

    if (collapse) {
        ui.bottomPanelLayout->removeWidget(ui.masterControlsPanel);
        ui.masterControlsPanel->setVisible(false);

        if (!canHandleNinjamPanels) {
            ui.bottomPanelLayout->addWidget(ui.masterMeter, 1, 1, 1, 1, Qt::AlignCenter);
            ui.masterMeter->setVisible(true);
        }
        else {
            ui.bottomPanelLayout->removeWidget(ui.masterMeter);
            ui.masterMeter->setVisible(false);

            auto metronomePanel = ninjamWindow->getMetronomePanel();
            ui.bottomPanelLayout->removeWidget(metronomePanel);
            metronomePanel->setVisible(false);
        }
    }
    else {
        ui.bottomPanelLayout->removeWidget(ui.masterMeter);

        ui.masterControlsLayout->addWidget(ui.masterMeter);
        ui.masterMeter->setVisible(true);

        if (canHandleNinjamPanels)
            addNinjamPanelsInBottom();
        else
            ui.bottomPanelLayout->addWidget(ui.masterControlsPanel, 1, 1, 1, 1, Qt::AlignCenter);

        ui.masterControlsPanel->setVisible(true);
    }
}

void MainWindow::addNinjamPanelsInBottom()
{
    auto ninjamPanel = ninjamWindow->getNinjamPanel();
    auto metronomePanel = ninjamWindow->getMetronomePanel();
    if (!ninjamPanel || !metronomePanel || bottomCollapsed)
        return;

    qCDebug(jtGUI) << "adding ninjam panels...";

    ui.bottomPanelLayout->removeWidget(ui.masterControlsPanel);

    ui.bottomPanelLayout->addWidget(metronomePanel, 1, 0, 1, 1);
    ui.bottomPanelLayout->addWidget(ninjamPanel, 1, 1, 1, 1, Qt::AlignCenter);
    ui.bottomPanelLayout->addWidget(ui.masterControlsPanel, 1, 2, 1, 1);

    ui.bottomPanelLayout->setAlignment(metronomePanel, Qt::AlignBottom | Qt::AlignCenter);
    ui.bottomPanelLayout->setAlignment(ui.masterControlsPanel, Qt::AlignBottom | Qt::AlignCenter);

    ui.masterTitleLabel->setVisible(true);
    ninjamPanel->setVisible(true);
    metronomePanel->setVisible(true);

}

void MainWindow::wireNinjamControllerSignals()
{
    auto controller = mainController->getNinjamController();
    connect(controller, &NinjamController::preparedToTransmit, this, &MainWindow::startTransmission);
    connect(controller, &NinjamController::preparingTransmission, this, &MainWindow::prepareTransmission);
    connect(controller, &NinjamController::currentBpiChanged, this, &MainWindow::updateBpi);
    connect(controller, &NinjamController::currentBpmChanged, this, &MainWindow::updateBpm);
    connect(controller, &NinjamController::intervalBeatChanged, this, &MainWindow::updateCurrentIntervalBeat);

    connect(controller, &NinjamController::userLeave, this, &MainWindow::handleUserLeaving);
    connect(controller, &NinjamController::userEnter, this, &MainWindow::handleUserEntering);

    connect(controller, &NinjamController::userBlockedInChat, this, &MainWindow::showFeedbackAboutBlockedUserInChat);
    connect(controller, &NinjamController::userUnblockedInChat, this, &MainWindow::showFeedbackAboutUnblockedUserInChat);

    connect(controller, &NinjamController::publicChatMessageReceived, this, &MainWindow::addMainChatMessage); // main chat
    connect(controller, &NinjamController::privateChatMessageReceived, this, &MainWindow::addPrivateChatMessage);

    connect(controller, &NinjamController::topicMessageReceived, this, [=](const QString &message){

        if (chatTabWidget)
            chatTabWidget->getMainChat()->setTopicMessage(message);
    });

    connect(controller, &NinjamController::started, this, [=]() {

        collapseBottomArea(bottomCollapsed);
        updateCollapseButtons();

    });

    Q_ASSERT(xmitInactivityDetector);
    xmitInactivityDetector->initialize(controller);

}

void MainWindow::setPrivateChatInputstatus(const QString userName, bool enabled)
{
    if (userName == JAMTABA_CHAT_BOT_NAME)
        return;

    auto chat = chatTabWidget->getPrivateChat(userName);
    if (chat) {
        chat->setInputsStatus(enabled);
    }
}

void MainWindow::handleUserLeaving(const QString &userName)
{
    auto chatPanel = chatTabWidget->getFocusedChatPanel();
    if (!chatPanel)
        return;

    auto localUser = mainController->getUserName();
    chatPanel->addMessage(localUser, JAMTABA_CHAT_BOT_NAME, tr("%1 has left the room.").arg(userName));

    setPrivateChatInputstatus(userName, false); // deactive the private chat when user leave

    usersColorsPool->giveBack(userName); // reuse the color mapped to this 'leaving' user
}

void MainWindow::handleUserEntering(const QString &userName)
{
    auto chatPanel = chatTabWidget->getFocusedChatPanel();
    if (!chatPanel)
        return;

    auto localUser = mainController->getUserName();
    chatPanel->addMessage(localUser, JAMTABA_CHAT_BOT_NAME, tr("%1 has joined the room.").arg(userName));

    setPrivateChatInputstatus(userName, true); // activate the chat if user is entering again
}

void MainWindow::showLastChordsInMainChat()
{
    Q_ASSERT(ninjamWindow);
    Q_ASSERT(chatTabWidget);

    auto loginService = mainController->getLoginService();
    auto roomInfo = ninjamWindow->getRoomInfo();

    QString lastChordProgression = loginService->getChordProgressionFor(roomInfo);
    ChatChordsProgressionParser parser;
    if (parser.containsProgression(lastChordProgression)) {
        ChordProgression progression = parser.parse(lastChordProgression);
        QString title = tr("Last chords used");

        auto mainChatPanel = chatTabWidget->getMainChat();
        Q_ASSERT(mainChatPanel);
        mainChatPanel->addLastChordsMessage(title, progression.toString());
        mainChatPanel->addChordProgressionConfirmationMessage(progression);

    }
}

void MainWindow::createVoteButton(const gui::chat::SystemVotingMessage &votingMessage)
{
    if (!votingMessage.isValidVotingMessage())
        return;

    Q_ASSERT(chatTabWidget);

    auto mainChatPanel = chatTabWidget->getMainChat();
    Q_ASSERT(mainChatPanel);

    quint32 voteValue = votingMessage.getVoteValue();
    quint32 expireTime = votingMessage.getExpirationTime();
    if (votingMessage.isBpiVotingMessage())
        mainChatPanel->addBpiVoteConfirmationMessage(voteValue, expireTime);
    else
        mainChatPanel->addBpmVoteConfirmationMessage(voteValue, expireTime);
}

void MainWindow::handleChordProgressionMessage(const User &user, const QString &message)
{
    Q_UNUSED(user)

    Q_ASSERT(chatTabWidget);

    ChatChordsProgressionParser parser;
    try{
        ChordProgression chordProgression = parser.parse(message);
        auto mainChatPanel = chatTabWidget->getMainChat();
        Q_ASSERT(mainChatPanel);
        mainChatPanel->addChordProgressionConfirmationMessage(chordProgression);
    }
    catch (const std::runtime_error &e) {
        qCritical() << e.what();
    }
}

bool MainWindow::canShowBlockButtonInChatMessage(const QString &userName) const
{
    /**
        Avoid the block button for bot and current user messages. Is not a good idea allow user
    to block yourself :).
        In vote messages (to change BPI or BPM) user name is empty. The last logic test is
    avoiding show block button in vote messages (fixing #389).

    **/

    auto ninjamController = mainController->getNinjamController();
    bool userIsBot = ninjamController->userIsBot(userName) || userName == JAMTABA_CHAT_BOT_NAME;
    bool currentUserIsPostingTheChatMessage = userName == mainController->getUserName(); // chat message author and the current user name are the same?
    return !userIsBot && !currentUserIsPostingTheChatMessage && !userName.isEmpty();
}

void MainWindow::addPrivateChatMessage(const User &remoteUser, const QString &message)
{
    if (!chatTabWidget->contains(remoteUser.getFullName())) {
        createPrivateChat(remoteUser.getName(), remoteUser.getIp(), false); // create new private chat, but not focused
    }
    else {
        auto privateChat = chatTabWidget->getPrivateChat(remoteUser.getFullName());
        if (privateChat && !privateChat->inputsAreEnabled())
            privateChat->setInputsStatus(true);
    }
    
    Q_ASSERT(chatTabWidget->contains(remoteUser.getFullName()));

    auto chatPanel = chatTabWidget->getPrivateChat(remoteUser.getFullName());
    if (chatPanel) {
        auto localUser = mainController->getUserName();
        chatPanel->addMessage(localUser, remoteUser.getName(), message, true, true);
    }
}

void MainWindow::addMainChatMessage(const User &msgAuthor, const QString &message)
{
    Q_ASSERT(chatTabWidget);
    Q_ASSERT(ninjamWindow);
    Q_ASSERT(chatTabWidget->getMainChat());

    QString remoteUserName = msgAuthor.getName();

    bool isSystemVoteMessage = gui::chat::parseSystemVotingMessage(message).isValidVotingMessage();

    bool isChordProgressionMessage = false;
    if (!isSystemVoteMessage) {
        ChatChordsProgressionParser chordsParser;
        isChordProgressionMessage = chordsParser.containsProgression(message);
    }

    bool showBlockButton = canShowBlockButtonInChatMessage(remoteUserName);
    bool showTranslationButton = !isChordProgressionMessage;

    auto mainChatPanel = chatTabWidget->getMainChat();
    Q_ASSERT(mainChatPanel);

    auto localUserName = mainController->getUserName();
    mainChatPanel->addMessage(localUserName, remoteUserName, message, showTranslationButton, showBlockButton);

    static bool localUserWasVotingInLastMessage = false;

    if (isSystemVoteMessage) {
        auto voteMessage = gui::chat::parseSystemVotingMessage(message);

        QTimer *expirationTimer = voteMessage.isBpiVotingMessage() ? bpiVotingExpiratonTimer : bpmVotingExpirationTimer;

        bool isFirstSystemVoteMessage = gui::chat::isFirstSystemVotingMessage(remoteUserName, message);
        if (isFirstSystemVoteMessage) { //starting a new votation round
            if (!localUserWasVotingInLastMessage) {  //don't create the vote button if local user is proposing BPI or BPM change
                createVoteButton(voteMessage);
            }
            else { //if local user is proposing a bpi/bpm change the combos are disabled until the voting reach majority or expire
                if (voteMessage.isBpiVotingMessage())
                    ninjamWindow->setBpiComboPendingStatus(true);
                else
                    ninjamWindow->setBpmComboPendingStatus(true);
                if (QApplication::focusWidget()) //clear comboboxes focus when disabling
                    QApplication::focusWidget()->clearFocus();
            }
        }

        //timer is restarted in every vote
        expirationTimer->start(voteMessage.getExpirationTime() * 1000); //QTimer::start will cancel a previous voting expiration timer
    }
    else if (isChordProgressionMessage) {
        handleChordProgressionMessage(msgAuthor, message);
    }

    localUserWasVotingInLastMessage = gui::chat::isLocalUserVotingMessage(message) && msgAuthor.getName() == mainController->getUserName();
}

void MainWindow::addMainChatPanel()
{
    qCDebug(jtGUI) << "adding ninjam chat panel...";

    auto mainChatPanel = chatTabWidget->createPublicChat(createTextEditorModifier());

    mainChatPanel->setTintColor(tintColor);

    connect(mainChatPanel, &ChatPanel::userConfirmingChordProgression, this, &MainWindow::acceptChordProgression);
    connect(mainChatPanel, &ChatPanel::userSendingNewMessage, this, &MainWindow::sendNewChatMessage);
    connect(mainChatPanel, &ChatPanel::userConfirmingVoteToBpiChange, this, &MainWindow::voteToChangeBpi);
    connect(mainChatPanel, &ChatPanel::userConfirmingVoteToBpmChange, this, &MainWindow::voteToChangeBpm);
    connect(mainChatPanel, &ChatPanel::userBlockingChatMessagesFrom, this, &MainWindow::blockUserInChat);
    connect(mainChatPanel, &ChatPanel::fontSizeOffsetEdited, mainController, &MainController::storeChatFontSizeOffset);

    initializeVotingExpirationTimers();

    showLastChordsInMainChat();

    setChatsVisibility(true);

    updateCollapseButtons();
}

void MainWindow::createPrivateChat(const QString &remoteUserName, const QString &remoteUserIP, bool focusNewChat)
{
    QString userFullName = remoteUserName + "@" + remoteUserIP;

    if (remoteUserIP.isEmpty() || chatTabWidget->contains(userFullName))
        return;

    auto chatPanel = chatTabWidget->createPrivateChat(remoteUserName, remoteUserIP, createTextEditorModifier(), focusNewChat);
    Q_ASSERT(chatPanel);

    chatPanel->setTintColor(tintColor);

    chatPanel->setTopicMessage(tr("Private chat with %1").arg(remoteUserName));

    connect(chatPanel, &ChatPanel::userSendingNewMessage, this, [=](const QString &message) {

        auto ninjamController = mainController->getNinjamController();

        QString text = QString("/msg %1 %2")
                .arg(userFullName)
                .arg(message);

        ninjamController->sendChatMessage(text);

        auto localUserName = mainController->getUserName();
        chatPanel->addMessage(localUserName, localUserName, message, false);

    });

    connect(chatPanel, &ChatPanel::userBlockingChatMessagesFrom, this, &MainWindow::blockUserInChat);

    connect(chatPanel, &ChatPanel::fontSizeOffsetEdited, mainController, &MainController::storeChatFontSizeOffset);
}

void MainWindow::addPrivateChat(const QString &remoteUserName, const QString &userIP)
{
    createPrivateChat(remoteUserName, userIP, true);
}

void MainWindow::blockUserInChat(const QString &userNameToBlock)
{
    auto ninjamController = mainController->getNinjamController();
    auto user = ninjamController->getUserByName(userNameToBlock);
    if (user.getName() == userNameToBlock)
        ninjamController->blockUserInChat(user);
}

void MainWindow::voteToChangeBpi(int newBpi)
{
    if (mainController->isPlayingInNinjamRoom()) {
        auto controller = mainController->getNinjamController();
        if (controller)
            controller->voteBpi(newBpi);
    }
}

void MainWindow::voteToChangeBpm(int newBpm)
{
    if (mainController->isPlayingInNinjamRoom()) {
        auto controller = mainController->getNinjamController();
        if (controller)
            controller->voteBpm(newBpm);
    }
}

void MainWindow::sendNewChatMessage(const QString &msg)
{
    auto ninjamController = mainController->getNinjamController();
    if (ninjamController)
        ninjamController->sendChatMessage(msg);
}

void MainWindow::showFeedbackAboutBlockedUserInChat(const QString &userName)
{
    Q_ASSERT(chatTabWidget);

    auto chatPanel = chatTabWidget->getFocusedChatPanel();
    Q_ASSERT(chatPanel);
    chatPanel->removeMessagesFrom(userName);

    auto localUserName = mainController->getUserName();
    auto msgAuthor = JAMTABA_CHAT_BOT_NAME;
    chatPanel->addMessage(localUserName, msgAuthor, tr("%1 is blocked in the chat").arg(userName));
}

void MainWindow::showFeedbackAboutUnblockedUserInChat(const QString &userName)
{
    Q_ASSERT(chatTabWidget);

    auto chatPanel = chatTabWidget->getFocusedChatPanel();
    Q_ASSERT(chatPanel);

    auto localUserName = mainController->getUserName();
    auto msgAuthor = JAMTABA_CHAT_BOT_NAME;
    chatPanel->addMessage(localUserName, msgAuthor, tr("%1 is unblocked in the chat").arg(userName));
}

void MainWindow::enableLooperButtonInLocalTracks(bool enable)
{
    for (auto trackGroupView : localGroupChannels) {
        auto tracks = trackGroupView->getTracks<LocalTrackView *>();
        for (auto track : tracks) {
            track->enableLopperButton(enable);
        }
    }
}

void MainWindow::updateUserNameLineEditToolTip()
{
    bool readOnly = ui.userNameLineEdit->isReadOnly();

    QString toolTip = readOnly ? tr("Your name cannot be edited while jamming!") : tr(
        "Only Letters, numbers, hyphen and underscore allowed! Spaces will be replaced by an underscore.");

    ui.userNameLineEdit->setToolTip(toolTip);
    ui.labelUserNameIcon->setToolTip(toolTip);
}

void MainWindow::setUserNameReadOnlyStatus(bool readOnly)
{
    ui.userNameLineEdit->setReadOnly(readOnly);

    QString icon = readOnly ? QStringLiteral("pencil-read-only.png") : QStringLiteral("pencil.png");
    ui.labelUserNameIcon->setPixmap(QPixmap(":/images/" + icon));

    updateUserNameLineEditToolTip();

    if (readOnly) {
        if (ui.userNameLineEdit->hasFocus())
            ui.userNameLineEdit->clearFocus();
    }
}

void MainWindow::showMetronomePreferencesDialog()
{
    openPreferencesDialog(ui.actionMetronome);
}

// +++++++++++++++ PREPARING TO XMIT +++++++++++
// this signal is received when ninjam controller is ready to transmit (after the 'preparing' intervals).
void MainWindow::startTransmission()
{
    setInputTracksPreparingStatus(false); // tracks are prepared to transmit
}

void MainWindow::prepareTransmission()
{
    // tracks are waiting to start transmiting
    setInputTracksPreparingStatus(true);
}

void MainWindow::exitFromRoom(bool normalDisconnection, QString disconnectionMessage)
{
    hideBusyDialog();

    // unlock the user name field
    setUserNameReadOnlyStatus(false);

    // remove the jam room tab (the last tab)
    if (ui.contentTabWidget->count() > 1) {
        ui.contentTabWidget->widget(1)->deleteLater(); // delete the room window
        ui.contentTabWidget->removeTab(1);
    }

    // remove ninjam panel from main window
    if (ninjamWindow) {
        ui.bottomPanelLayout->removeWidget(ninjamWindow->getNinjamPanel());
        ui.bottomPanelLayout->removeWidget(ninjamWindow->getMetronomePanel());
    }

    collapseBottomArea(bottomCollapsed); // update bottom area visibility honoring collapse status

    hideChordsPanel();

    if (ninjamWindow) {
        ninjamWindow->deleteLater();
        ninjamWindow.reset(nullptr);
    }

    setChatsVisibility(false);

    setInputTracksPreparingStatus(false); /** reset the preparing status when user leave the room. This is specially necessary if user enter in a room and leave before the track is prepared to transmit.*/

    if (!normalDisconnection) {
        if (!disconnectionMessage.isEmpty())
            showMessageBox(tr("Error"), disconnectionMessage, QMessageBox::Warning);
        else
            showMessageBox(tr("Error"), tr("Disconnected from ninjam server"), QMessageBox::Warning);
    } else {
        if (roomToJump) { // waiting the disconnection to connect in a new room?
            showBusyDialog(tr("Connecting with %1").arg(roomToJump->getName()));
            mainController->enterInRoom(*roomToJump, getChannelsNames(),
                                        (passwordToJump.isNull()
                                         || passwordToJump.isEmpty()) ? "" : passwordToJump);
            roomToJump.reset();
            passwordToJump = "";
        }
    }

    updatePublicRoomsListLayout();

    enableLooperButtonInLocalTracks(false); // disable looper buttons when exiting from server

    closeAllLooperWindows();

    for (LocalTrackGroupView *trackGroup : localGroupChannels) {
        for (LocalTrackView *trackView : trackGroup->getTracks<LocalTrackView*>()) {
            trackView->getInputNode()->stopLooper();
        }
    }

    if (xmitInactivityDetector)
        xmitInactivityDetector->deinitialize();

}

void MainWindow::closeAllLooperWindows()
{
    for (LooperWindow *looperWindow : looperWindows.values()) {
        if (looperWindow) {
            looperWindow->close();
            looperWindow->detachCurrentLooper();
            looperWindow->deleteLater();
        }
    }
    looperWindows.clear();
}

void MainWindow::toggleChatCollapseStatus()
{
    Q_ASSERT(chatTabWidget);
    Q_ASSERT(mainController);

    if (!mainController->isPlayingInNinjamRoom() || !chatTabWidget->isVisible())
        return;

    chatTabWidget->toggleCollapse();
}

void MainWindow::setChatsVisibility(bool chatVisible)
{
    if (!chatTabWidget)
        return;

    if (chatVisible) {
        ui.gridLayout->addWidget(chatTabWidget, 0, 2);
    }
    else {
        ui.gridLayout->removeWidget(chatTabWidget);
        chatTabWidget->clear();
    }

    chatTabWidget->setVisible(chatVisible);

    updateCollapseButtons();

    // adjust bottom panel colspan
    int colSpan = chatVisible ? 2 : 1;
    ui.gridLayout->addWidget(ui.bottomPanel, 1, 1, 1, colSpan);
}

void MainWindow::setInputTracksPreparingStatus(bool preparing)
{
    for (LocalTrackGroupView *trackGroup : localGroupChannels) {
        trackGroup->setPreparingStatus(preparing);
    }
}

void MainWindow::timerEvent(QTimerEvent *)
{
    if (!mainController)
        return;

    // update local input track peaks
    for (TrackGroupView *channel : localGroupChannels)
        channel->updateGuiElements();

    // update metronome peaks
    if (mainController->isPlayingInNinjamRoom()) {
        // update tracks peaks
        if (ninjamWindow)
            ninjamWindow->updatePeaks();
    }

    // update cpu and memmory usage
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - lastPerformanceMonitorUpdate >= PERFORMANCE_MONITOR_REFRESH_TIME) {

        if (performanceMonitorLabel)
            performanceMonitorLabel->setText(QString("MEM: %1%").arg(performanceMonitor->getMemmoryUsed()));

        lastPerformanceMonitorUpdate = now;
    }

    // prevent screen saver if user is playing
    if (mainController->isPlayingInNinjamRoom())
        screensaverBlocker->update();

    // update public server stream plot
    if (mainController->isPlayingRoomStream()) {
        long long roomID = mainController->getCurrentStreamingRoomID();
        JamRoomViewPanel *roomView = roomViewPanels[roomID];
        if (roomView) {
            bool buffering = mainController->getRoomStreamer()->isBuffering();
            roomView->setShowBufferingState(buffering);
            if (!buffering) {
                auto peak = mainController->getRoomStreamPeak();
                roomView->addPeak(peak.getMaxPeak());
            } else {
                int percentage = mainController->getRoomStreamer()->getBufferingPercentage();
                roomView->setBufferingPercentage(percentage);
            }
        }
    }

    // update looper window sound waves
    for (auto looperWindow : looperWindows.values()) {
        if (looperWindow && looperWindow->isVisible()) {
            looperWindow->updateDrawings();
        }
    }

    // update master peaks
    auto masterPeak = mainController->getMasterPeak();
    ui.masterMeter->setPeak(masterPeak.getLeftPeak(), masterPeak.getRightPeak(),
                            masterPeak.getLeftRMS(), masterPeak.getRightRMS());

    // update all blinkable buttons
    BlinkableButton::updateAllBlinkableButtons();

    // update network transfer rate labels
    static qint64 lastNetworkTransferRateUpdate = QDateTime::currentMSecsSinceEpoch();

    bool showNetworkUsageLabels = mainController->isPlayingInNinjamRoom();
    transmitTransferRateLabel->setVisible(showNetworkUsageLabels);
    receiveTransferRateLabel->setVisible(showNetworkUsageLabels);
    receiveIcon->setVisible(showNetworkUsageLabels);
    transmitIcon->setVisible(showNetworkUsageLabels);

    if (mainController->isPlayingInNinjamRoom()) {
        const qint64 ellapsedTime = now - lastNetworkTransferRateUpdate;
        if (ellapsedTime >= networkUsageUpdatePeriod) {
            QString transmitTransferRate = QString::number(mainController->getTotalUploadTransferRate() / 1024 * 8);
            transmitTransferRateLabel->setText(transmitTransferRate.leftJustified(3, QChar(' ')));
            QString transmitText = QString("%1 %2 Kbps")
                                            .arg(tr("Uploading"))
                                            .arg(transmitTransferRate);
            transmitTransferRateLabel->setToolTip(transmitText);
            transmitIcon->setToolTip(transmitTransferRateLabel->toolTip());

            QString receiveTransferRate = QString::number(mainController->getTotalDownloadTransferRate() / 1024 * 8);
            receiveTransferRateLabel->setText(receiveTransferRate.leftJustified(3, QChar(' ')));
            QString receiveText = QString("%1 %2 Kbps")
                                            .arg(tr("Downloading"))
                                            .arg(receiveTransferRate);
            receiveTransferRateLabel->setToolTip(receiveText);
            receiveIcon->setToolTip(transmitTransferRateLabel->toolTip());

            lastNetworkTransferRateUpdate = now;
        }
    }

}

void MainWindow::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev)

    centerBusyDialog();

    if (ninjamWindow) {
        NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
        if (ninjamPanel) {
            bool usingLargeWindow = width() >= MAIN_WINDOW_MIN_SIZE.width();
            ninjamPanel->maximizeControlsWidget(usingLargeWindow);
        }
    }
}

void MainWindow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::WindowStateChange && mainController) {
        mainController->storeWindowSettings(isMaximized(), computeLocation(), size());
    }
    else if (ev->type() == QEvent::LanguageChange) {
        ui.retranslateUi(this); // translate MainWindow.ui static texts

        updateUserNameLineEditToolTip(); // translate user name line edit tool tip

        translateCollapseButtonsToolTips();

        if (ninjamWindow && chatTabWidget)
            chatTabWidget->updatePublicChatTabTitle(); // translate the chat tab title
    }

    QMainWindow::changeEvent(ev);
}

void MainWindow::translateCollapseButtonsToolTips()
{
    if (buttonCollapseBottomArea) {
        buttonCollapseBottomArea->setToolTip(tr("Collapse bottom area"));
        buttonCollapseChat->setToolTip(tr("Collapse chat"));
        buttonCollapseLocalChannels->setToolTip(tr("Collapse local channels"));
    }
}

QPointF MainWindow::computeLocation() const
{
    QRect screen = QApplication::desktop()->screenGeometry();
    float x = (float)this->pos().x()/screen.width();
    float y = (float)this->pos().y()/screen.height();
    return QPointF(x, y);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (mainController) {
        mainController->storeWindowSettings(isMaximized(), computeLocation(), size());
        mainController->setChatSectionCollapsed(chatTabWidget->isCollapsed());
    }

    closeAllFloatingWindows();
}

MainWindow::~MainWindow()
{
    qCDebug(jtGUI) << "MainWindow destructor...";
    setParent(nullptr);

    if (mainController)
        mainController->stop();

    for (LocalTrackGroupView *groupView : this->localGroupChannels)
        groupView->detachMainControllerInSubchannels();

    mainController = nullptr;

    killTimer(timerID);
    qCDebug(jtGUI) << "Main frame timer killed!";
    qCDebug(jtGUI) << "MainWindow destructor finished.";
}

void MainWindow::connectInPrivateServer(const QString &server, int serverPort,
                                            const QString &userName, const QString &password)
{
    mainController->storePrivateServerSettings(server, serverPort, password);
    mainController->setUserName(userName);
    ui.userNameLineEdit->setText(userName);

    login::RoomInfo roomInfo(server, serverPort, login::RoomTYPE::NINJAM, 32, 32);
    tryEnterInRoom(roomInfo, password);
}

void MainWindow::showJamtabaIssuesWebPage()
{
    openUrlInUserBrowser("https://github.com/elieserdejesus/JamTaba/issues");
}

void MainWindow::showJamtabaWikiWebPage()
{
    openUrlInUserBrowser("https://github.com/elieserdejesus/JamTaba/wiki");
}

void MainWindow::showJamtabaUsersManual()
{
    QString url = "https://github.com/elieserdejesus/JamTaba/wiki/Jamtaba's-user-guide";
    openUrlInUserBrowser(url);
}

void MainWindow::showJamtabaTranslators()
{
    QString url = "http://www.jamtaba.com/#translators";
    openUrlInUserBrowser(url);
}

void MainWindow::openUrlInUserBrowser(const QString &url)
{
    if (!QDesktopServices::openUrl(QUrl(url))) {
        QMessageBox::warning(this, tr("Error"), tr("Can't open the link!"), QMessageBox::Cancel, QMessageBox::Ok );
    }
}

void MainWindow::showPrivateServerWindow()
{
    if (!privateServerWindow) {
        privateServerWindow.reset(new PrivateServerWindow());
    }

    if (!privateServerWindow->isVisible())
        privateServerWindow->show();
    
    privateServerWindow->raise();
    privateServerWindow->activateWindow();
}

void MainWindow::showConnectWithPrivateServerDialog()
{
    auto privateServerDialog = new PrivateServerDialog(ui.centralWidget, mainController);

    connect(privateServerDialog, &PrivateServerDialog::connectionAccepted, this, &MainWindow::connectInPrivateServer);

    centerDialog(privateServerDialog);
    privateServerDialog->show();
    privateServerDialog->exec();
}

void MainWindow::centerDialog(QWidget *dialog)
{
    QPoint globalPosition = mapToGlobal(dialog->parentWidget()->pos());
    QSize parentSize = dialog->parentWidget()->size();
    int x = globalPosition.x() + parentSize.width()/2 - dialog->width()/2;
    int y = globalPosition.y();
    if (dialog->height() >= parentSize.height()/2) // big dialog like preferences panel
        y += parentSize.height()/2 - dialog->height()/2;
    else // small dialogs
        y += parentSize.height()/3;

    dialog->move(dialog->mapFromGlobal(QPoint(x, y)));
}

void MainWindow::showNinjamCommunityWebPage()
{
    QDesktopServices::openUrl(QUrl("http://www.ninbot.com"));
}

void MainWindow::showNinjamOfficialWebPage()
{
    QDesktopServices::openUrl(QUrl("http://www.cockos.com/ninjam/"));
}

// preferences menu
void MainWindow::openPreferencesDialog(QAction *action)
{
    if (action == ui.actionQuit) { // TODO put the Quit action in a better place. A 'Quit' entry in the 'Preferences' menu is not good. A Quit action is not related with Preferences.
        close();
    } else {
        PreferencesDialog::PreferencesTab initialTab = PreferencesDialog::TabMultiTrackRecording;
        if (action == ui.actionAudioPreferences)
            initialTab = PreferencesDialog::TabAudio;
        else if (action == ui.actionMidiPreferences)
            initialTab = PreferencesDialog::TabMidi;
        else if (action == ui.actionVstPreferences)
            initialTab = PreferencesDialog::TabVST;
        else if (action == ui.actionMetronome)
            initialTab = PreferencesDialog::TabMetronome;
        else if (action == ui.actionLooper)
            initialTab = PreferencesDialog::TabLooper;
        else if (action == ui.actionRemember)
            initialTab = PreferencesDialog::TabRemember;

        stopCurrentRoomStream();

        PreferencesDialog *dialog = createPreferencesDialog(); // factory method, overrided in derived classes MainWindowStandalone and MainWindowVST

        qDebug(jtGUI) << "Initializing preferences dialog";
        dialog->initialize(initialTab, &mainController->getSettings(), mainController->getJamRecoders());// initializing here to avoid call virtual methods inside PreferencesDialog constructor

        qCDebug(jtGUI) << "Showing preferences dialog";
        dialog->show();
        dialog->exec();
    }
}

void MainWindow::setupPreferencesDialogSignals(PreferencesDialog *dialog)
{
    Q_ASSERT(dialog);

    connect(dialog, &PreferencesDialog::multiTrackRecordingStatusChanged, this, &MainWindow::setMultiTrackRecordingStatus);

    connect(dialog, &PreferencesDialog::jamRecorderStatusChanged, this, &MainWindow::setJamRecorderStatus);

    connect(dialog, &PreferencesDialog::recordingPathSelected, this, &MainWindow::setRecordingPath);

    connect(dialog, &PreferencesDialog::builtInMetronomeSelected, this, &MainWindow::setBuiltInMetronome);

    connect(dialog, &PreferencesDialog::customMetronomeSelected, this, &MainWindow::setCustomMetronome);

    connect(dialog, &PreferencesDialog::encodingQualityChanged, mainController, &MainController::setEncodingQuality);

    connect(dialog, &PreferencesDialog::looperAudioEncodingFlagChanged, mainController, &MainController::storeLooperAudioEncodingFlag);

    connect(dialog, &PreferencesDialog::looperFolderChanged, mainController, &MainController::storeLooperFolder);

    connect(dialog, &PreferencesDialog::looperWaveFilesBitDepthChanged, mainController, &MainController::storeLooperBitDepth);

    connect(dialog, &PreferencesDialog::rememberRemoteUserSettingsChanged, mainController, &MainController::storeRemoteUserRememberSettings);
    connect(dialog, &PreferencesDialog::rememberCollapsibleSectionsSettingsChanged, mainController, &MainController::storeCollapsibleSectionsRememberSettings);
}

void MainWindow::setBuiltInMetronome(const QString &metronomeAlias)
{
    mainController->setBuiltInMetronome(metronomeAlias);
}

void MainWindow::setCustomMetronome(const QString &primaryBeatFile,
                                    const QString &offBeatFile, const QString &accentBeatFile)
{
    mainController->setCustomMetronome(primaryBeatFile, offBeatFile, accentBeatFile);
}

void MainWindow::setMultiTrackRecordingStatus(bool recording)
{
    mainController->storeMultiTrackRecordingStatus(recording);
}

void MainWindow::setJamRecorderStatus(const QString &writerId, bool status)
{
    mainController->storeJamRecorderStatus(writerId, status);
}

void MainWindow::setRecordingPath(const QString &newRecordingPath)
{
    mainController->storeMultiTrackRecordingPath(newRecordingPath);
}

void MainWindow::initializeViewMenu()
{
    connect(ui.menuMetering, &QMenu::aboutToShow, this, &MainWindow::updateMeteringMenu);

    connect(ui.menuMetering, &QMenu::triggered, this, &MainWindow::handleMenuMeteringAction);

    QActionGroup *meteringActionGroup = new QActionGroup(this);
    meteringActionGroup->addAction(ui.actionShowPeaksOnly);
    meteringActionGroup->addAction(ui.actionShowRmsOnly);
    meteringActionGroup->addAction(ui.actionShowPeakAndRMS);
}

void MainWindow::handleMenuMeteringAction(QAction *action)
{
    if (action == ui.actionShowMaxPeaks){
        AudioMeter::setPaintMaxPeakMarker(ui.actionShowMaxPeaks->isChecked());
    }
    else{
        if (action == ui.actionShowPeakAndRMS){
            AudioMeter::paintPeaksAndRms();
        }
        else if (action == ui.actionShowPeaksOnly){
            AudioMeter::paintPeaksOnly();
        }
        else{ // RMS only
            AudioMeter::paintRmsOnly();
        }
    }
    quint8 meterOption = 0; // rms + peaks
    if (AudioMeter::isPaintingPeaksOnly())
        meterOption = 1;
    else if (AudioMeter::isPaintingRmsOnly())
        meterOption = 2;

    mainController->storeMeteringSettings(AudioMeter::isPaintintMaxPeakMarker(), meterOption);
}

void MainWindow::updateMeteringMenu()
{
    ui.actionShowMaxPeaks->setChecked(AudioMeter::isPaintintMaxPeakMarker());
    bool showingPeakAndRms = AudioMeter::isPaintingRMS() && AudioMeter::isPaintingPeaks();
    if (showingPeakAndRms) {
        ui.actionShowPeakAndRMS->setChecked(true);
    }
    else{
        ui.actionShowPeaksOnly->setChecked(AudioMeter::isPaintingPeaks());
        ui.actionShowRmsOnly->setChecked(AudioMeter::isPaintingRMS());
    }
}

void MainWindow::updatePublicRoomsListLayout()
{
    QList<login::RoomInfo> roomInfos;

    for (auto roomView : roomViewPanels)
        roomInfos.append(roomView->getRoomInfo());

    refreshPublicRoomsList(roomInfos);
}

QSize MainWindow::getSanitizedWindowSize(const QSize &size, const QSize &minimumSize) const
{
    // fixing #343. If MainWindow::showFullScreen() is called after a setMinimumSize(), and the current
    // minimum size is less then desktop size the fullScreen is buggy because we are forcing an
    // impossible minimum size (the minimum size is too large). This code is ensuring the minimum
    // window size is a valid window size.
    QDesktopWidget desktop;

    QSize screenSize = desktop.availableGeometry().size();

    static int topBarHeight = 0;
    if (topBarHeight == 0) // when the window is fullscreen the topBarHeight is zero (no title window).
        topBarHeight = frameGeometry().height() - geometry().height(); // geometry is the 'window client area', frameGeometry contain the window title bar area. http://doc.qt.io/qt-4.8/application-windows.html#window-geometry

    const int minimumWidth = minimumSize.width();
    const int maximumWidth = screenSize.width();
    const int minimumHeight = minimumSize.height();
    const int maximumHeight = screenSize.height() - topBarHeight;

    QSize finalSize(size);
    if (finalSize.width() < minimumWidth)
        finalSize.setWidth(minimumWidth);
    else
        if (finalSize.width() > maximumWidth)
            finalSize.setWidth(maximumWidth);

    if (finalSize.height() < minimumHeight)
        finalSize.setHeight(minimumHeight);
    else
        if (finalSize.height() > maximumHeight)
            finalSize.setHeight(maximumHeight);

    return finalSize;
}

void MainWindow::initializeWindowSize()
{
    setMinimumSize(getMinimumWindowSize());

    if (!isMaximized() && !isFullScreen()) {
        QSize lastSize = mainController->getSettings().getLastWindowSize();
        QSize newSize = getSanitizedWindowSize(lastSize, minimumSize());
        resize(newSize);
        showNormal();
    }

    const int spaces = 6;
    ui.tabLayout->setContentsMargins(spaces, spaces, spaces, spaces);
    ui.allRoomsContent->layout()->setSpacing(spaces);

    // local tracks are narrowed in mini mode if user is using more than 1 subchannel
    bool usingSmallWindow = width() < MAIN_WINDOW_MIN_SIZE.width();
    for (LocalTrackGroupView *localTrackGroup : localGroupChannels) {
        if (usingSmallWindow && (localTrackGroup->getTracksCount() > 1 || localGroupChannels.size() > 1))
            localTrackGroup->setToNarrow();
        else
            localTrackGroup->setToWide();
    }

}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        if (target == ui.localTracksWidget) {
            updateLocalInputChannelsGeometry();
            return true;
        }
        else if (target == ui.contentTabWidget) {
            updatePublicRoomsListLayout();
            return false;
        }
    }

    return QMainWindow::eventFilter(target, event);
}

void MainWindow::resetLocalChannels()
{
    for (LocalTrackGroupView *localChannel : localGroupChannels) {
        localChannel->resetTracks();
    }
}

void MainWindow::setTransmitingStatus(int channelID, bool xmitStatus)
{
    mainController->setTransmitingStatus(channelID, xmitStatus);
}

bool MainWindow::isTransmiting(int channelID) const
{
    return mainController->isTransmiting(channelID);
}

void MainWindow::showJamtabaCurrentVersion()
{
    QString title = tr("About Jamtaba");
    QString text = tr("Jamtaba version is %1").arg(QApplication::applicationVersion());
    QMessageBox *box = new QMessageBox();
    box->setWindowIcon(this->windowIcon());
    box->setWindowTitle(title);
    box->setText(text);
    box->deleteLater();
    box->exec();
}

void MainWindow::setMasterGain(int faderPosition)
{
    float newGain = faderPosition/100.0;
    mainController->setMasterGain(newGain);
}

ChordsPanel *MainWindow::createChordsPanel()
{
    ChordsPanel *chordsPanel = new ChordsPanel();
    connect(chordsPanel, &ChordsPanel::sendingChordsToChat, this, &MainWindow::sendCurrentChordProgressionToChat);
    connect(chordsPanel, &ChordsPanel::chordsDiscarded, this, &MainWindow::hideChordsPanel);

    return chordsPanel;
}

void MainWindow::acceptChordProgression(const ChordProgression &progression)
{
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    if (progression.canBeUsed(currentBpi)) {
        if (!chordsPanel)
            chordsPanel = createChordsPanel();
        else
            chordsPanel->setVisible(true);

        bool needStrech = progression.getBeatsPerInterval() != currentBpi;
        if (needStrech)
            chordsPanel->setChords(progression.getStretchedVersion(currentBpi));
        else
            chordsPanel->setChords(progression);

        // add the chord panel in top of bottom panel in main window
        ui.bottomPanelLayout->addWidget(chordsPanel, 0, 0, 1, 3);

        if (ninjamWindow) {
            NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
            ninjamPanel->setLowContrastPaintInIntervalPanel(true);
        }

        sendAcceptedChordProgressionToServer(progression);

    } else {
        int measures = progression.getMeasures().size();
        QString msg = tr("These chords (%1 measures) can't be used in a %2 bpi interval!")
                      .arg(QString::number(measures))
                      .arg(QString::number(currentBpi));
        QMessageBox::warning(this, tr("Problem..."), msg);
    }
}

void MainWindow::sendAcceptedChordProgressionToServer(const ChordProgression &progression)
{
    auto service = mainController->getLoginService();
    auto currentRoom = ninjamWindow->getRoomInfo();
    QString chords = progression.toString();
    bool chordProgressionIsOutdated = service->getChordProgressionFor(currentRoom) != chords;
    if (chordProgressionIsOutdated) {
        QString userName = mainController->getUserName();
        QString serverName = currentRoom.getName();
        quint32 serverPort = currentRoom.getPort();
        service->sendChordProgressionToServer(userName, serverName, serverPort, chords);
    }
}

void MainWindow::sendCurrentChordProgressionToChat()
{
    if (chordsPanel && mainController) { // just in case
        ChordProgression chordProgression = chordsPanel->getChordProgression();
        mainController->getNinjamController()->sendChatMessage(chordProgression.toString());
    }
}

void MainWindow::hideChordsPanel()
{
    if (chordsPanel) {
        ui.bottomPanel->layout()->removeWidget(chordsPanel);
        chordsPanel->setVisible(false);
        chordsPanel->deleteLater();
        chordsPanel = nullptr;
    }

    if (ninjamWindow)
        ninjamWindow->getNinjamPanel()->setLowContrastPaintInIntervalPanel(false);
}

// ninjam controller events
void MainWindow::updateBpi(int bpi)
{
    if (!ninjamWindow)
        return;

    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();

    if (!ninjamPanel)
        return;

    ninjamPanel->setBpi(bpi);
    if (chordsPanel) {
        bool bpiWasAccepted = chordsPanel->setBpi(bpi);
        if (!bpiWasAccepted)
            hideChordsPanel();
    }
}

void MainWindow::updateBpm(int bpm)
{
    if (!ninjamWindow)
        return;

    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    if (!ninjamPanel)
        return;

    ninjamPanel->setBpm(bpm);
}

void MainWindow::updateCurrentIntervalBeat(int beat)
{
    if (!ninjamWindow)
        return;

    NinjamPanel *ninjamPanel = ninjamWindow->getNinjamPanel();
    if (!ninjamPanel)
        return;

    ninjamPanel->setCurrentBeat(beat);

    if (chordsPanel)
        chordsPanel->setCurrentBeat(beat);
}

void MainWindow::setupWidgets()
{
    ui.masterMeter->setOrientation(Qt::Horizontal);

    setChatsVisibility(false); // hide chat area until connect in a server to play

    if (ui.allRoomsContent->layout())
        delete ui.allRoomsContent->layout();

    ui.allRoomsContent->setLayout(new QGridLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0, 0, 0, 0);

    ui.localTracksWidget->installEventFilter(this);

    QString lastUserName = mainController->getUserName();

    if (!lastUserName.isEmpty())
        ui.userNameLineEdit->setText(lastUserName);

    ui.masterTitleLabel->setVisible(false);

    chatTabWidget = new ChatTabWidget(this, mainController, usersColorsPool.data());
    connect(chatTabWidget, &ChatTabWidget::collapsedChanged, this, &MainWindow::chatCollapseChanged);

    setChatsVisibility(false);

    // remember chat collapse status
    auto settings = mainController->getSettings();
    if (settings.isRememberingChatSection()) {
        chatTabWidget->collapse(settings.isChatSectionCollapsed());
    }

    ui.gridLayout->addWidget(chatTabWidget, 0, 2, 1, 1);
}

void MainWindow::setupSignals()
{
    connect(ui.menuPreferences, &QMenu::triggered, this, &MainWindow::openPreferencesDialog);

    connect(ui.actionNinjam_community_forum, &QAction::triggered, this, &MainWindow::showNinjamCommunityWebPage);

    connect(ui.actionNinjam_Official_Site, &QAction::triggered, this, &MainWindow::showNinjamOfficialWebPage);

    connect(ui.actionConnectWithPrivateServer, &QAction::triggered, this, &MainWindow::showConnectWithPrivateServerDialog);

    connect(ui.actionHostPrivateServer, &QAction::triggered, this, &MainWindow::showPrivateServerWindow);

    connect(ui.actionReportBugs, &QAction::triggered, this, &MainWindow::showJamtabaIssuesWebPage);

    connect(ui.actionWiki, &QAction::triggered, this, &MainWindow::showJamtabaWikiWebPage);

    connect(ui.actionUsersManual, &QAction::triggered, this, &MainWindow::showJamtabaUsersManual);

    connect(ui.actionTranslators, &QAction::triggered, this, &MainWindow::showJamtabaTranslators);

    connect(ui.actionCurrentVersion, &QAction::triggered, this, &MainWindow::showJamtabaCurrentVersion);

    connect(ui.localControlsCollapseButton, &QPushButton::clicked, this, &MainWindow::toggleLocalTracksCollapseStatus);

    connect(mainController->getRoomStreamer(), &AbstractMp3Streamer::error, this, &MainWindow::handlePublicRoomStreamError);

    connect(ui.masterFader, &Slider::valueChanged, this, &MainWindow::setMasterGain);

    connect(ui.menuLanguage, &QMenu::triggered, this, &MainWindow::setLanguage);

    connect(ui.userNameLineEdit, &UserNameLineEdit::textChanged, this, &MainWindow::updateUserName);

    connect(mainController, &controller::MainController::themeChanged, this, &MainWindow::handleThemeChanged);

    ui.contentTabWidget->installEventFilter(this);

}

void MainWindow::updateUserName()
{
    QString newUserName = ui.userNameLineEdit->text();
    mainController->setUserName(newUserName);
}

void MainWindow::initializeMasterFader()
{
    ui.masterFader->setSliderType(Slider::AudioSlider);

    float lastMasterGain = mainController->getSettings().getLastMasterGain();
    int faderPosition = lastMasterGain * 100;
    ui.masterFader->setValue(faderPosition);
    setMasterGain(faderPosition);
}

void MainWindow::closeAllFloatingWindows()
{
    if (!mainController)
        return;

    if (mainController->isPlayingInNinjamRoom() && ninjamWindow)
        ninjamWindow->closeMetronomeFloatingWindow();

    closeAllLooperWindows();

    if (privateServerWindow)
        privateServerWindow->close();
}

void MainWindow::initializeVotingExpirationTimers()
{
    if (!ninjamWindow)
        return;

    bpiVotingExpiratonTimer = new QTimer(this);
    bpmVotingExpirationTimer = new QTimer(this);
    bpiVotingExpiratonTimer->setSingleShot(true);
    bpmVotingExpirationTimer->setSingleShot(true);

    connect(bpiVotingExpiratonTimer, &QTimer::timeout, ninjamWindow.data(), &NinjamRoomWindow::resetBpiComboBox);
    connect(bpmVotingExpirationTimer, &QTimer::timeout, ninjamWindow.data(), &NinjamRoomWindow::resetBpmComboBox);
}

void MainWindow::initializeCollapseButtons()
{

    Q_ASSERT(buttonCollapseLocalChannels);

    connect(buttonCollapseLocalChannels, &QPushButton::clicked, this, &MainWindow::toggleLocalTracksCollapseStatus);

    connect(buttonCollapseBottomArea, &QPushButton::clicked, this, &MainWindow::toggleBottomAreaCollapseStatus);

    connect(buttonCollapseChat, &QPushButton::clicked, this, &MainWindow::toggleChatCollapseStatus);

    updateCollapseButtons();
}

void MainWindow::chatCollapseChanged(bool chatCollapsed)
{
    updateCollapseButtons();

    mainController->setChatSectionCollapsed(chatCollapsed); // update the persistence status
}

void MainWindow::updateCollapseButtons()
{
    Q_ASSERT(buttonCollapseBottomArea);

    buttonCollapseBottomArea->setChecked(!bottomCollapsed);

    if (chatTabWidget)
        buttonCollapseChat->setChecked(!chatTabWidget->isCollapsed());

    buttonCollapseChat->setEnabled(mainController->isPlayingInNinjamRoom());

    buttonCollapseLocalChannels->setChecked(!ui.localControlsCollapseButton->isChecked());

}

QString MainWindow::getChannelGroupName(int index) const
{
    return localGroupChannels.at(index)->getGroupName();
}
