#include "MainWindow.h"
#include "LooperWindow.h"
#include "LocalTrackView.h"
#include "LocalTrackGroupView.h"
#include "NinjamRoomWindow.h"
#include "ninjam/client/Service.h"
#include "UserNameDialog.h"
#include "JamRoomViewPanel.h"
#include "widgets/MapWidget.h"
#include "widgets/ChatTabWidget.h"
#include "PreferencesDialog.h"
#include "PrivateServerDialog.h"
#include "PrivateServerWindow.h"
#include "chords/ChordsPanel.h"
#include "chords/ChatChordsProgressionParser.h"
#include "chords/ChordProgressionCreationDialog.h"
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
#include "chat/NinjamChatMessageParser.h"
#include "loginserver/MainChat.h"
#include "TextEditorModifier.h"
#include "widgets/InstrumentsMenu.h"
#include "ninjam/client/Types.h"

// to get versions
#include "libavutil/avutil.h"
#include "vorbis/codec.h"
#include "miniupnpc.h"

#include <QDesktopWidget>
#include <QDesktopServices>
#include <QRect>
#include <QDateTime>
#include <QImage>
#include <QCameraInfo>
#include <QToolTip>

const QSize MainWindow::MAIN_WINDOW_MIN_SIZE = QSize(1100, 685);
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
using login::MainChat;
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
    mainChat(new MainChat()),
    ninjamWindow(nullptr),
    roomToJump(nullptr),
    performanceMonitor(new PerformanceMonitor()),
    lastPerformanceMonitorUpdate(0)
{
    qCDebug(jtGUI) << "Creating MainWindow...";

    ui.setupUi(this);

    setWindowTitle(QString("JamTaba (%1 bits)").arg(QSysInfo::WordSize));

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

    if (ui.localControlsCollapseButton->isChecked()) {
        cameraCombo->setVisible(false);
    }
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
        QSize bestResolution = getBestCameraResolution(resolutions);
        settings.setResolution(bestResolution);
        camera->setViewfinderSettings(settings);
        qDebug() << "Setting camera viewFinder resolution to " << camera->viewfinderSettings().resolution();

        //getBestSupportedFrameRate();

        mainController->setVideoProperties(bestResolution);
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

QSize MainWindow::getBestCameraResolution(const QList<QSize> resolutions) const
{
    auto bestResolution = MainController::MAX_VIDEO_SIZE;

    if (!resolutions.isEmpty()) {
        auto lowestResolution = resolutions.first();
        if (lowestResolution.width() > MainController::MAX_VIDEO_SIZE.width()) { // using the lowest resolution in big resolution cams
            bestResolution = lowestResolution;
        }
        else { // pick the first resolution where width < 320
            for (int i = resolutions.size() - 1; i >= 0;  --i) {
                if (resolutions.at(i).width() <= MainController::MAX_VIDEO_SIZE.width()) {
                    bestResolution = resolutions.at(i);
                    break;
                }
            }
            //bestResolution = resolutions.first();
        }
    }

    return bestResolution;
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

            if (localGroupChannels.size() > 1) {
                auto secondChannel = localGroupChannels.at(1);
                if (secondChannel && secondChannel->isVideoChannel()) {
                    removeChannelsGroup(secondChannel->getChannelIndex());
                }
            }

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
    else { // if the camera is correctly activated we need need create a 2nd channel to xmit the video

        while (localGroupChannels.size() > 1)
            removeChannelsGroup(localGroupChannels.at(1)->getChannelIndex()); // delete the 2nd channel if exists

        addChannelsGroup(-1); // add the 2nd channel using the default icon

        auto secondChannel = localGroupChannels.at(1);
        secondChannel->setPeakMeterMode(localGroupChannels.first()->isShowingPeakMeterOnly());
        if (secondChannel) {
            auto channelIndex = secondChannel->getChannelIndex();
            auto voiceChatActivated = mainController->isVoiceChatActivated(channelIndex);
            if (voiceChatActivated) {
                mainController->setVoiceChatStatus(channelIndex, false);
            }

            secondChannel->setAsVideoChannel();
        }
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
            cameraView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
            cameraView->setMaximumHeight(136);

            connect(cameraView, &VideoWidget::statusChanged, this, &MainWindow::changeCameraStatus);
        }

        if (!cameraLayout) {
            cameraLayout = new QVBoxLayout();

            cameraCombo = new QComboBox();
            cameraCombo->setObjectName(QStringLiteral("cameraCombo"));
            connect(cameraCombo, SIGNAL(activated(int)), this, SLOT(selectNewCamera(int)));
            cameraCombo->setVisible(false);
            cameraCombo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

            cameraLayout->addWidget(cameraView, 1, Qt::AlignHCenter);
            cameraLayout->addWidget(cameraCombo);

            auto leftPanelLayout = static_cast<QVBoxLayout *>(ui.leftPanel->layout());
            leftPanelLayout->addLayout(cameraLayout);
            leftPanelLayout->setAlignment(cameraLayout, Qt::AlignCenter);
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
        auto frame = videoFrameGrabber->grab();
        cameraView->setCurrentFrame(frame);

        // scale the grabed frame if is bigger than MAX_VIDEO_SIZE. This is necessary because some cameras
        // have only big resolutions, and we have problems sensing big resolution videos to ninjam servers.

        if (frame.width() > MainController::MAX_VIDEO_SIZE.width())
            return frame.scaled(mainController->getVideoResolution(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        return frame;
    }

    return QImage();
}

void MainWindow::initializeMeteringOptions()
{
    const auto &settings = mainController->getSettings();
    AudioSlider::setPaintMaxPeakMarker(settings.isShowingMaxPeaks());
    quint8 meterOption = settings.getMeterOption();
    switch (meterOption) {
    case 0:
        AudioSlider::paintPeaksAndRms();
        break;
    case 1:
        AudioSlider::paintPeaksOnly();
        break;
    case 2:
        AudioSlider::paintRmsOnly();
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

    ui.chatTabWidget->setPreferredTranslationLanguage(locale);

    if (mainController->isPlayingInNinjamRoom()) {
        ninjamWindow->updateGeoLocations();
    }
}

void MainWindow::handleThemeChanged()
{
    QString themeName = mainController->getTheme();
    MapWidget::setNightMode(MainWindow::themeCanUseNightModeWorldMaps(themeName));

    ui.masterFader->updateStyleSheet();

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

    ui.chatTabWidget->setChatsTintColor(color);

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
            themeName = "Navy_nm"; // fallback to Navy theme
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

    auto publicChatActivated = settings.publicChatIsActivated();
    createMainChat(publicChatActivated);

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
        cameraCombo->setVisible(cameraView->isVisible() && cameraCombo->count() > 1);
        
        if (showPeakMetersOnly) {
            cameraCombo->setVisible(false);
        }
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
        Channel channel(trackGroupView->getInstrumentIcon());
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
        QString roomID = mainController->getCurrentStreamingRoomID();

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

            if (channel->isVideoChannel())
                cameraView->activate(false); // deactivate the camera if the 2nd channel is deleted

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

void MainWindow::addChannelsGroup(int instrumentIndex)
{
    int channelIndex = localGroupChannels.size();
    addLocalChannel(channelIndex, instrumentIndex, true);

    if (mainController->isPlayingInNinjamRoom()) {
        mainController->sendNewChannelsNames(getChannelsMetadata());

        // create an encoder for this channel in next interval
        bool voiceChannelActivated = mainController->isVoiceChatActivated(channelIndex);
        auto ninjamController = mainController->getNinjamController();
        ninjamController->scheduleEncoderChangeForChannel(channelIndex, voiceChannelActivated);

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
    mainController->sendNewChannelsNames(getChannelsMetadata());
}

// This is a factory method and is overrided in derived classes to create more specific instances.
LocalTrackGroupView *MainWindow::createLocalTrackGroupView(int channelGroupIndex)
{
    return new LocalTrackGroupView(channelGroupIndex, this);
}

LocalTrackGroupView *MainWindow::addLocalChannel(int channelGroupIndex, int instrumentIndex,
                                                 bool createFirstSubchannel)
{
    auto localChannel = createLocalTrackGroupView(channelGroupIndex);

    connect(localChannel, &LocalTrackGroupView::instrumentIconChanged, this, &MainWindow::updateChannelsNames);

    connect(localChannel, &LocalTrackGroupView::trackAdded, this, &MainWindow::updateLocalInputChannelsGeometry);

    connect(localChannel, &LocalTrackGroupView::trackRemoved, this, &MainWindow::updateLocalInputChannelsGeometry);

    if (!localGroupChannels.isEmpty()) // the second channel?
        localChannel->setPreparingStatus(localGroupChannels.at(0)->isPreparingToTransmit());

    localGroupChannels.append(localChannel);

    localChannel->setInstrumentIcon(instrumentIndex);
    ui.localTracksLayout->addWidget(localChannel);

    if (createFirstSubchannel) {
         localChannel->addTrackView(channelGroupIndex);
    }

    localChannel->setTintColor(tintColor);

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

        auto channel = localGroupChannels.at(inputTrack->getChanneGroupIndex());
        Q_ASSERT(channel);

        int subchannelInternalIndex = channel->getSubchannelInternalIndex(trackID);
        QString channelName = channel->getChannelGroupName();
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

        // just a temporary workaround to https://github.com/elieserdejesus/JamTaba/issues/1104
        if(channelIndex > 0 && channel.instrumentIndex == static_cast<int>(InstrumentIndex::Video))
            continue; // skip this channel, it's a video channel used in the last session

        qCDebug(jtGUI) << "\tCreating channel "<< channelIndex;
        bool createFirstSubChannel = channel.subChannels.isEmpty();
        auto channelView = addLocalChannel(channelIndex, channel.instrumentIndex, createFirstSubChannel);
        for (const auto &subChannel : channel.subChannels) {
            qCDebug(jtGUI) << "\t\tCreating sub-channel ";
            auto subChannelView = channelView->addTrackView(channelIndex);
            initializeLocalSubChannel(subChannelView, subChannel);
        }

        channelIndex++;
    }
    if (channelIndex == 0) // no channels in settings file or no settings file...
        addLocalChannel(0, -1, true); // create a channel using an empty instrument icon

    qCDebug(jtGUI) << "Initializing local inputs done!";

    QApplication::restoreOverrideCursor();
}

void MainWindow::initializeLoginService()
{
    auto loginService = this->mainController->getLoginService();

    connect(loginService, &LoginService::roomsListAvailable, this, &MainWindow::refreshPublicRoomsList);

    connect(loginService, &LoginService::newVersionAvailableForDownload, this, &MainWindow::showNewVersionAvailableMessage);
}

void MainWindow::closeContentTab(int index)
{
    if (index > 0) { // the first tab (rooms to play) is not closeable
        if (index == 1) { // jam tab
            showBusyDialog(tr("disconnecting ..."));
            if (mainController->isPlayingInNinjamRoom())
                mainController->stopNinjamController();
        }
        else if (index == 2) { // chords tab
            auto chordsPanel = ui.contentTabWidget->widget(2);
            if (chordsPanel) {
                ui.contentTabWidget->removeTab(2);
                chordsPanel->deleteLater();
            }
        }
    }
}

void MainWindow::changeTab(int index)
{
    if (index == 1) { // click in jam tab?
        if (mainController->isPlayingInNinjamRoom() && mainController->isPlayingRoomStream()) {
            stopCurrentRoomStream();
        }
    } else {
        if (index == 0) { // click in the public rooms tab?
            updatePublicRoomsListLayout();
        }
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

void MainWindow::showNewVersionAvailableMessage(const QString &versionTag, const QString &publicationDate, const QString &latestVersionDetails)
{
    hideBusyDialog();
    QString text = tr("A new Jamtaba version is available for download! Please use the <a href='http://www.jamtaba.com'>new version</a>!");

    auto locale = QLocale::system();
    auto dateFormat = locale.dateTimeFormat(QLocale::FormatType::ShortFormat);
    auto formatedDate = locale.toString(QDateTime::fromString(publicationDate, Qt::ISODate), dateFormat);

    if (!latestVersionDetails.isEmpty()) {
        text += "<br><br>";
        text += QString("<b>%1</b> (release date: %2)<br>").arg(versionTag).arg(formatedDate);
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
        int rowIndex = twoCollumns ? (index / 2) : (index);
        int collumnIndex = twoCollumns ? (index % 2) : 0;
        auto roomViewPanel = roomViewPanels[roomInfo.getUniqueName()];
        if (roomViewPanel) {
            roomViewPanel->refresh(roomInfo);
            // check if is playing a public room stream but this room is empty now
            if (mainController->isPlayingRoomStream()) {
                if (roomInfo.isEmpty() && mainController->getCurrentStreamingRoomID() == roomInfo.getUniqueName()) {
                    stopCurrentRoomStream();
                }
            }
            ui.allRoomsContent->layout()->removeWidget(roomViewPanel); // the widget is removed but added again
        } else {
            roomViewPanel = createJamRoomViewPanel(roomInfo);
            roomViewPanels.insert(roomInfo.getUniqueName(), roomViewPanel);
        }
        auto layout = dynamic_cast<QGridLayout *>(ui.allRoomsContent->layout());
        layout->addWidget(roomViewPanel, rowIndex, collumnIndex);
        index++;
    }

    if (mainController->isPlayingInNinjamRoom())
        this->ninjamWindow->updateGeoLocations();

    /** updating country flag and country names after refresh the public rooms list. This is necessary because the call to webservice used to get country codes and  country names is not synchronous. So, if country code and name are not cached we receive these data from the webservice after some seconds.*/
}

void MainWindow::playPublicRoomStream(const login::RoomInfo &roomInfo)
{
    // clear all plots
    for (auto viewPanel : this->roomViewPanels.values())
        viewPanel->clear(roomInfo.getUniqueName() != viewPanel->getRoomInfo().getUniqueName());

    if (roomInfo.hasStream()) // just in case...
        mainController->playRoomStream(roomInfo);
}

void MainWindow::stopPublicRoomStream(const login::RoomInfo &roomInfo)
{
    Q_UNUSED(roomInfo)
    stopCurrentRoomStream();
}

QList<ninjam::client::ChannelMetadata> MainWindow::getChannelsMetadata() const
{
    QList<ninjam::client::ChannelMetadata> channelsMetadata;

    for (auto channelGroup : localGroupChannels) {
        ninjam::client::ChannelMetadata channelData;
        channelData.name = channelGroup->getChannelGroupName();
        channelData.voiceChatActivated = mainController->isVoiceChatActivated(channelGroup->getChannelIndex());
        channelsMetadata.append(channelData);
    }

    return channelsMetadata;
}

// user trying enter in a room
void MainWindow::tryEnterInRoom(const login::RoomInfo &roomInfo, const QString &password)
{
    // stop room stream before enter in a room
    if (mainController->isPlayingRoomStream()) {
        auto roomID = mainController->getCurrentStreamingRoomID();

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
        mainController->enterInRoom(roomInfo, getChannelsMetadata(), password);
    }
}

void MainWindow::enterInRoom(const login::RoomInfo &roomInfo)
{
    qCDebug(jtGUI) << "hidding busy dialog...";
    hideBusyDialog();

    setUserNameReadOnlyStatus(true); // lock the user name field, user name can't be changed when jamming

    qCDebug(jtGUI) << "creating NinjamRoomWindow...";
    ninjamWindow.reset(createNinjamWindow(roomInfo, mainController));

    auto tabText = QString("%1 [%2]").arg(roomInfo.getName()).arg(roomInfo.getPort());
    auto index = ui.contentTabWidget->addTab(ninjamWindow.data(), tabText);
    ui.contentTabWidget->setCurrentIndex(index);

    auto serverName = gui::sanitizeServerName(roomInfo.getName());
    createNinjamServerChat(serverName);

    auto settings = mainController->getSettings();
    ui.chatTabWidget->collapse(settings.isRememberingChatSection() && settings.isChatSectionCollapsed());

    addNinjamPanelsInBottom();

    ui.leftPanel->adjustSize();

    wireNinjamSignals();

    enableLooperButtonInLocalTracks(true); // looper buttons are enabled when entering in a server

    qCDebug(jtGUI) << "MainWindow::enterInRoom() done!";
}

void MainWindow::collapseBottomArea(bool collapse)
{
    bool canHandleNinjamPanels = mainController->isPlayingInNinjamRoom() && ninjamWindow && ninjamWindow->getNinjamPanel() && ninjamWindow->getMetronomePanel();

    if (canHandleNinjamPanels) {
        auto ninjamPanel = ninjamWindow->getNinjamPanel();
        ninjamPanel->setVisible(true);
        ninjamPanel->setCollapseMode(collapse);

        if (collapse)
            ui.bottomPanelLayout->addWidget(ninjamPanel, 1, 0, ui.bottomPanelLayout->rowCount(), ui.bottomPanelLayout->columnCount()); // re-add ninjamPanel using 3 cols for colspan
    }

    ui.masterTitleLabel->setVisible(!collapse);

    if (collapse) {
        if (canHandleNinjamPanels) {

            ui.masterControlsPanel->setVisible(false);

            auto metronomePanel = ninjamWindow->getMetronomePanel();
            ui.bottomPanelLayout->removeWidget(metronomePanel);
            metronomePanel->setVisible(false);
        }
    }
    else {
        ui.bottomPanelLayout->removeWidget(ui.masterFader);

        ui.masterControlsLayout->addWidget(ui.masterFader);
        ui.masterControlsPanel->setVisible(true);

        if (canHandleNinjamPanels)
            addNinjamPanelsInBottom();
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

void MainWindow::wireNinjamSignals()
{
    auto controller = mainController->getNinjamController();
    connect(controller, &NinjamController::preparedToTransmit, this, &MainWindow::startTransmission);
    connect(controller, &NinjamController::preparingTransmission, this, &MainWindow::prepareTransmission);
    connect(controller, &NinjamController::currentBpiChanged, this, &MainWindow::updateBpi);
    connect(controller, &NinjamController::currentBpmChanged, this, &MainWindow::updateBpm);
    connect(controller, &NinjamController::intervalBeatChanged, this, &MainWindow::updateCurrentIntervalBeat);

    connect(controller, &NinjamController::userLeave, this, &MainWindow::handleUserLeaving);
    connect(controller, &NinjamController::userEnter, this, &MainWindow::handleUserEntering);

    connect(controller, &NinjamController::publicChatMessageReceived, this, &MainWindow::addNinjamServerChatMessage); // main chat
    connect(controller, &NinjamController::privateChatMessageReceived, this, &MainWindow::addPrivateChatMessage);

    connect(controller, &NinjamController::topicMessageReceived, this, [=](const QString &message) {

        ui.chatTabWidget->getNinjamServerChat()->setTopicMessage(message);
    });

    connect(controller, &NinjamController::started, this, [=]() {

        collapseBottomArea(bottomCollapsed);
        updateCollapseButtons();

    });

    Q_ASSERT(xmitInactivityDetector);
    xmitInactivityDetector->initialize(controller);

    if (ninjamWindow) {
        auto chordsDialog = ninjamWindow->getChordProgressionDialog();
        connect(chordsDialog, &ChordProgressionCreationDialog::chordProgressionCreated, this, &MainWindow::acceptChordProgression);
    }

}

void MainWindow::setPrivateChatInputstatus(const QString userName, bool enabled)
{
    if (userName == JAMTABA_CHAT_BOT_NAME)
        return;

    auto chat = ui.chatTabWidget->getPrivateChat(userName);
    if (chat) {
        chat->setInputsStatus(enabled);
    }
}

void MainWindow::handleUserLeaving(const QString &userFullName)
{
    auto ninjamChat = ui.chatTabWidget->getNinjamServerChat();
    if (!ninjamChat)
        return;

    auto chatsToReport = QList<ChatPanel *>();
    chatsToReport.append(ninjamChat);

    auto privateChat = ui.chatTabWidget->getPrivateChat(userFullName);
    if (privateChat) {
        chatsToReport.append(privateChat);
        setPrivateChatInputstatus(userFullName, false); // deactive the private chat when user leave
    }

    auto localUser = mainController->getUserName();
    for (auto chat : chatsToReport)
        chat->addMessage(localUser, JAMTABA_CHAT_BOT_NAME, tr("%1 has left the room.").arg(ninjam::client::extractUserName(userFullName)));

    usersColorsPool->giveBack(userFullName); // reuse the color mapped to this 'leaving' user
}

void MainWindow::handleUserEntering(const QString &userFullName)
{
    auto ninjamChat = ui.chatTabWidget->getNinjamServerChat();
    if (!ninjamChat)
        return;

    auto chatsToReport = QList<ChatPanel *>();
    chatsToReport.append(ninjamChat);

    auto privateChat = ui.chatTabWidget->getPrivateChat(userFullName);
    if (privateChat) {
        chatsToReport.append(privateChat);
        setPrivateChatInputstatus(userFullName, true); // active the private chat when user enter
    }

    auto localUser = mainController->getUserName();
    for (auto chat : chatsToReport)
        chat->addMessage(localUser, JAMTABA_CHAT_BOT_NAME, tr("%1 has joined the room.").arg(ninjam::client::extractUserName(userFullName)));

}

void MainWindow::createVoteButton(const gui::chat::SystemVotingMessage &votingMessage)
{
    if (!votingMessage.isValidVotingMessage())
        return;

    auto ninjamChatPanel = ui.chatTabWidget->getNinjamServerChat();
    Q_ASSERT(ninjamChatPanel);

    quint32 voteValue = votingMessage.getVoteValue();
    quint32 expireTime = votingMessage.getExpirationTime();
    if (votingMessage.isBpiVotingMessage())
        ninjamChatPanel->addBpiVoteConfirmationMessage(voteValue, expireTime);
    else
        ninjamChatPanel->addBpmVoteConfirmationMessage(voteValue, expireTime);
}

void MainWindow::handleChordProgressionMessage(const User &user, const QString &message)
{
    Q_UNUSED(user)

    ChatChordsProgressionParser parser;
    try{
        ChordProgression chordProgression = parser.parse(message);
        auto ninjamChatPanel = ui.chatTabWidget->getNinjamServerChat();
        Q_ASSERT(ninjamChatPanel);
        ninjamChatPanel->addChordProgressionConfirmationMessage(chordProgression);
    }
    catch (const std::runtime_error &e) {
        qCritical() << e.what();
    }
}

bool MainWindow::canShowBlockButtonInChatMessage(const QString &userFullName) const
{
    /**
        Avoid the block button for bot and current user messages. Is not a good idea allow user
    to block yourself :).
        In vote messages (to change BPI or BPM) user name is empty. The last logic test is
    avoiding show block button in vote messages (fixing #389).

    **/

    auto userIsBot = false;

    auto ninjamController = mainController->getNinjamController();
    if (ninjamController)
        userIsBot = ninjamController->userIsBot(userFullName) || userFullName == JAMTABA_CHAT_BOT_NAME;

    bool currentUserIsPostingTheChatMessage = ninjam::client::extractUserName(userFullName) == mainController->getUserName(); // chat message author and the current user name are the same?
    return !userIsBot && !currentUserIsPostingTheChatMessage && !userFullName.isEmpty();
}



void MainWindow::addPrivateChatMessage(const User &remoteUser, const QString &message)
{
    if (!ui.chatTabWidget->contains(remoteUser.getFullName())) {
        createPrivateChat(remoteUser.getName(), remoteUser.getIp(), false); // create new private chat, but not focused
    }
    else {
        auto privateChat = ui.chatTabWidget->getPrivateChat(remoteUser.getFullName());
        if (privateChat && !privateChat->inputsAreEnabled())
            privateChat->setInputsStatus(true);
    }
    
    Q_ASSERT(ui.chatTabWidget->contains(remoteUser.getFullName()));

    auto chatPanel = ui.chatTabWidget->getPrivateChat(remoteUser.getFullName());
    if (chatPanel) {
        auto localUser = mainController->getUserName();

        if (!gui::chat::isServerInvitation(message)) {
            chatPanel->addMessage(localUser, remoteUser.getFullName(), message, true, true);
        }
        else { // handling server invitation as private message
            auto msg = gui::chat::parseServerInviteMessage(message);

            chatPanel->addMessage(localUser, remoteUser.getFullName(), msg.message, true, true);

            chatPanel->createServerInviteButton(msg.serverIP, msg.serverPort);
        }
    }
}

void MainWindow::addNinjamServerChatMessage(const User &msgAuthor, const QString &message)
{
    Q_ASSERT(ninjamWindow);
    Q_ASSERT(ui.chatTabWidget->getNinjamServerChat());

    auto remoteUserName = msgAuthor.getFullName();
    auto localUserName = mainController->getUserName();

    if (gui::chat::isNinbotLevelMessage(message)) {
        auto messageUserName = gui::chat::extractUserNameFromNinbotLevelMessage(message);
        if (messageUserName != localUserName)
            return; // skip all ninbot level messages sended to other musicians (only the messages sent to me will appear in the chat)
    }

    bool isSystemVoteMessage = gui::chat::parseSystemVotingMessage(message).isValidVotingMessage();

    bool isChordProgressionMessage = false;
    if (!isSystemVoteMessage) {
        ChatChordsProgressionParser chordsParser;
        isChordProgressionMessage = chordsParser.containsProgression(message);
    }

    bool showBlockButton = canShowBlockButtonInChatMessage(remoteUserName);
    bool showTranslationButton = !isChordProgressionMessage;

    auto mainChatPanel = ui.chatTabWidget->getNinjamServerChat();
    Q_ASSERT(mainChatPanel);

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

QString MainWindow::buildServerInviteMessage(const QString &serverIP, quint16 serverPort, bool isPrivateServer, bool showPrivateServerIpAndPort)
{
    auto msg = tr("Let's play in %1 : %2 ?").arg(serverIP).arg(serverPort);
    if (isPrivateServer) {
        msg = tr("Let's play in my private server?");
        if (showPrivateServerIpAndPort)
            msg += QString("\n\n IP: %1 \n\n PORT: %2 \n").arg(serverIP).arg(serverPort);
    }

    return msg;
}

void MainWindow::createMainChat(bool turnedOn)
{
    qCDebug(jtGUI) << "adding main chat panel...";

    auto mainChatPanel = ui.chatTabWidget->createMainChat(createTextEditorModifier());

    mainChatPanel->setTintColor(tintColor);

    connect(mainChatPanel, &ChatPanel::userSendingNewMessage, mainChat.data(), &MainChat::sendPublicMessage);

    connect(mainChat.data(), &MainChat::messageReceived, [=](const QString &userFullName, const QString &content){
        if (!mainController->userIsBlockedInChat(userFullName)) {
            auto localUserName = mainController->getUserName();
            bool showBlockButton = canShowBlockButtonInChatMessage(userFullName);
            mainChatPanel->addMessage(localUserName, userFullName, content, true, showBlockButton);
        }
    });

    connect(mainChat.data(), &MainChat::usersListChanged, ui.chatTabWidget, &ChatTabWidget::setConnectedUsersInMainChat);

    connect(mainChatPanel, &ChatPanel::userBlockingChatMessagesFrom, mainController, &MainController::blockUserInChat);
    connect(mainChatPanel, &ChatPanel::userUnblockingChatMessagesFrom, mainController, &MainController::unblockUserInChat);
    //connect(mainChatPanel, &ChatPanel::fontSizeOffsetEdited, mainController, &MainController::storeChatFontSizeOffset);

    connect(mainChatPanel, &ChatPanel::connectedUserContextMenuActivated, this, &MainWindow::fillConnectedUserContextMenu);

    connect(mainChatPanel, &ChatPanel::fontSizeOffsetEdited, mainController, &MainController::storeChatFontSizeOffset);

    connect(mainChat.data(), &MainChat::serverInviteReceived, [=](const QString &senderFullName, const QString &serverIP, quint16 serverPort, bool isPrivateServer){

        auto localUserName = mainController->getUserName();
        bool showBlockButton = true;
        auto msg = buildServerInviteMessage(serverIP, serverPort, isPrivateServer, false);
        mainChatPanel->addMessage(localUserName, senderFullName, msg, true, showBlockButton);

        mainChatPanel->createServerInviteButton(serverIP, serverPort);
    });

    connect(mainChatPanel, &ChatPanel::userAcceptingServerInvite, [=](const QString &serverIP, quint16 serverPort){
        tryEnterInRoom(login::RoomInfo(serverIP, serverPort, 8));
    });

    updateCollapseButtons();

    mainChatPanel->setInputsStatus(false);

    connect(mainChat.data(), &MainChat::connected, [=](){

        auto localUserName = mainController->getUserName();
        mainChat->setUserName(localUserName);

        auto msg = tr("Connected with public chat!");
        mainChatPanel->addMessage(localUserName, JAMTABA_CHAT_BOT_NAME, msg);

        mainChatPanel->turnOn();
        mainChatPanel->showConnectedUsersWidget(true);
    });

    connect(mainChat.data(), &MainChat::disconnected, [=](){
        if (mainController) { // if still running
            mainChatPanel->setInputsStatus(false);
            auto localUserName = mainController->getUserName();
            auto author = JAMTABA_CHAT_BOT_NAME;
            mainChatPanel->addMessage(localUserName, author, tr("Public chat disconnected!"), true, false);
        }
    });

    connect(mainChat.data(), &MainChat::error, [=](const QString &errorMessage){
        if (mainController) {
            auto localUserName = mainController->getUserName();
            auto author = JAMTABA_CHAT_BOT_NAME;
            mainChatPanel->addMessage(localUserName, author, errorMessage, true, false);
        }
    });

    connect(mainController, &MainController::userBlockedInChat, [=](const QString &userFullName){
        mainChatPanel->setConnectedUserBlockedStatus(userFullName, true);
    });

    connect(mainController, &MainController::userUnblockedInChat, [=](const QString &userFullName){
        mainChatPanel->setConnectedUserBlockedStatus(userFullName, false);
    });

    connect(mainChatPanel, &ChatPanel::turnedOn, this, &MainWindow::connectInMainChat);
    connect(mainChatPanel, &ChatPanel::turnedOff, [=](){
         mainChat->disconnectFromServer();
         mainChatPanel->showConnectedUsersWidget(false);
         mainController->setPublicChatActivated(false);
    });

    if (turnedOn)
        mainChatPanel->turnOn();
    else
        mainChatPanel->turnOff();
}

void MainWindow::connectInMainChat()
{
    auto publicChat = ui.chatTabWidget->getPublicChat();
    Q_ASSERT(publicChat);

    publicChat->addMessage(mainController->getUserName(), JAMTABA_CHAT_BOT_NAME, tr("Connecting ..."));

    mainChat->connectWithServer(MainChat::MAIN_CHAT_URL);

    mainController->setPublicChatActivated(true);
}

void MainWindow::fillUserContextMenu(QMenu &menu, const QString &userFullName, bool addInvitationEntry)
{
    auto userName = ninjam::client::extractUserName(userFullName);

    if (addInvitationEntry && mainController->isPlayingInNinjamRoom()) {

        auto service = mainController->getNinjamService();
        if (service) {
            auto serverInfo = service->getCurrentServer();
            if (serverInfo) {
                bool userIsAlreadyConnected = false;
                for (auto user : serverInfo->getUsers()) {
                    if (ninjam::client::maskIpInUserFullName(user.getFullName()) == ninjam::client::maskIpInUserFullName(userFullName)) {
                        if (user.hasActiveChannels()) {
                            userIsAlreadyConnected = true;
                            break;
                        }
                    }
                }

                if (!userIsAlreadyConnected) { // skip the invite menu entry if user is already connected in the server

                    QString inviteText = tr("Invite %1 to play in %2 [%3]")
                                    .arg(userName)
                                    .arg(serverInfo->getHostName())
                                    .arg(serverInfo->getPort());

                    bool isPrivateServer = false;
                    if (privateServerWindow && privateServerWindow->serverIsRunning()) {
                        isPrivateServer = serverInfo->getHostName() == privateServerWindow->getServerExternalIP();
                    }

                    auto action = menu.addAction(inviteText);
                    connect(action, &QAction::triggered, [=](){
                        mainChat->sendServerInvite(userFullName, serverInfo->getHostName(), serverInfo->getPort(), isPrivateServer);
                    });

                    menu.addSeparator();
                }
            }
        }
    }

    auto userIsBlocked = mainController->userIsBlockedInChat(userFullName);

    auto blockAction = menu.addAction(tr("Block %1 in chat").arg(userName));
    blockAction->setData(userFullName);
    blockAction->setEnabled(!userIsBlocked);
    connect(blockAction, &QAction::triggered, this, &MainWindow::blockUserInChat);

    auto unblockAction = menu.addAction(tr("Unblock %1 in chat").arg(userName));
    connect(unblockAction, &QAction::triggered, this, &MainWindow::unblockUserInChat);
    unblockAction->setData(userFullName);
    unblockAction->setEnabled(userIsBlocked);
}

void MainWindow::fillConnectedUserContextMenu(QMenu &menu, const QString &userFullName)
{
    if (ninjam::client::extractUserName(userFullName) != mainController->getUserName()) // not generating context menu to local user (avoid user blocking yourself or inviting yourseld)
        fillUserContextMenu(menu, userFullName, true);
}

void MainWindow::blockUserInChat()
{
    auto action  = qobject_cast<QAction *>(QObject::sender());
    if (action) {
        auto userFullName = action->data().toString();
        mainController->blockUserInChat(userFullName);
    }
}

void MainWindow::unblockUserInChat()
{
    auto action  = qobject_cast<QAction *>(QObject::sender());
    if (action) {
        auto userFullName = action->data().toString();
        mainController->unblockUserInChat(userFullName);
    }
}

void MainWindow::createNinjamServerChat(const QString &serverName)
{
    qCDebug(jtGUI) << "adding ninjam chat panel...";

    auto ninjamChatPanel = ui.chatTabWidget->createNinjamServerChat(serverName, createTextEditorModifier());

    ninjamChatPanel->setTintColor(tintColor);
    ninjamChatPanel->showConnectedUsersWidget(false);
    ninjamChatPanel->turnOn();
    ninjamChatPanel->hideOnOffButton();

    connect(ninjamChatPanel, &ChatPanel::userConfirmingChordProgression, this, &MainWindow::acceptChordProgression);
    connect(ninjamChatPanel, &ChatPanel::userSendingNewMessage, this, &MainWindow::sendChatMessageToNinjamServer);
    connect(ninjamChatPanel, &ChatPanel::userConfirmingVoteToBpiChange, this, &MainWindow::voteToChangeBpi);
    connect(ninjamChatPanel, &ChatPanel::userConfirmingVoteToBpmChange, this, &MainWindow::voteToChangeBpm);
    connect(ninjamChatPanel, &ChatPanel::userBlockingChatMessagesFrom, mainController, &MainController::blockUserInChat);
    connect(ninjamChatPanel, &ChatPanel::fontSizeOffsetEdited, mainController, &MainController::storeChatFontSizeOffset);

    initializeVotingExpirationTimers();

    updateCollapseButtons();
}

void MainWindow::createPrivateChat(const QString &remoteUserName, const QString &remoteUserIP, bool focusNewChat)
{
    QString userFullName = remoteUserName + "@" + remoteUserIP;

    if (remoteUserIP.isEmpty() || ui.chatTabWidget->contains(userFullName))
        return;

    auto chatPanel = ui.chatTabWidget->createPrivateChat(remoteUserName, remoteUserIP, createTextEditorModifier(), focusNewChat);
    Q_ASSERT(chatPanel);

    chatPanel->setTintColor(tintColor);
    chatPanel->showConnectedUsersWidget(false);
    chatPanel->turnOn();
    chatPanel->hideOnOffButton();

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

    connect(chatPanel, &ChatPanel::userAcceptingServerInvite, [=](const QString &serverIP, quint16 serverPort){
        tryEnterInRoom(login::RoomInfo(serverIP, serverPort, 8));
    });

    connect(chatPanel, &ChatPanel::userBlockingChatMessagesFrom, mainController, &MainController::blockUserInChat);

    connect(chatPanel, &ChatPanel::fontSizeOffsetEdited, mainController, &MainController::storeChatFontSizeOffset);
}

void MainWindow::addPrivateChat(const QString &remoteUserName, const QString &userIP)
{
    createPrivateChat(remoteUserName, userIP, true);
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

void MainWindow::sendChatMessageToNinjamServer(const QString &msg)
{
    auto ninjamController = mainController->getNinjamController();
    if (ninjamController)
        ninjamController->sendChatMessage(msg);
}

void MainWindow::showFeedbackAboutBlockedUserInChat(const QString &userFullName)
{
    // remote all blocked user messages
    auto chatPanel = ui.chatTabWidget->getFocusedChatPanel();
    Q_ASSERT(chatPanel);
    chatPanel->removeMessagesFrom(userFullName);

    // add a message in chat about the blocked user
    auto localUserName = mainController->getUserName();
    auto msgAuthor = JAMTABA_CHAT_BOT_NAME;
    auto blockedUserName = ninjam::client::extractUserName(userFullName);
    chatPanel->addMessage(localUserName, msgAuthor, tr("%1 is blocked in the chat").arg(blockedUserName));
}

void MainWindow::showFeedbackAboutUnblockedUserInChat(const QString &userFullName)
{
    auto chatPanel = ui.chatTabWidget->getFocusedChatPanel();
    Q_ASSERT(chatPanel);

    auto localUserName = mainController->getUserName();
    auto msgAuthor = JAMTABA_CHAT_BOT_NAME;
    auto unblockedUserName = ninjam::client::extractUserName(userFullName);
    chatPanel->addMessage(localUserName, msgAuthor, tr("%1 is unblocked in the chat").arg(unblockedUserName));
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

    // remove jam and the chords tab
    while (ui.contentTabWidget->count() > 1) {
        ui.contentTabWidget->widget(1)->deleteLater();
        ui.contentTabWidget->removeTab(1);
    }

    // remove ninjam panel from main window
    if (ninjamWindow) {
        ui.bottomPanelLayout->removeWidget(ninjamWindow->getNinjamPanel());
        ui.bottomPanelLayout->removeWidget(ninjamWindow->getMetronomePanel());
    }

    collapseBottomArea(bottomCollapsed); // update bottom area visibility honoring collapse status

    if (ninjamWindow) {
        ninjamWindow->deleteLater();
        ninjamWindow.reset(nullptr);
    }

    setInputTracksPreparingStatus(false); /** reset the preparing status when user leave the room. This is specially necessary if user enter in a room and leave before the track is prepared to transmit.*/

    if (!normalDisconnection) {
        if (!disconnectionMessage.isEmpty())
            showMessageBox(tr("Error"), disconnectionMessage, QMessageBox::Warning);
        else
            showMessageBox(tr("Error"), tr("Disconnected from ninjam server"), QMessageBox::Warning);
    } else {
        if (roomToJump) { // waiting the disconnection to connect in a new room?
            showBusyDialog(tr("Connecting with %1").arg(roomToJump->getName()));
            mainController->enterInRoom(*roomToJump, getChannelsMetadata(),
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

    ui.chatTabWidget->closeNinjamChats();

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

    // update cpu and RAM usage
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - lastPerformanceMonitorUpdate >= PERFORMANCE_MONITOR_REFRESH_TIME) {

        if (performanceMonitorLabel) {

                   auto memmoryUsed = performanceMonitor->getMemmoryUsed();
                   auto batteryUsed = performanceMonitor->getBatteryUsed();

                   bool showMemmory = memmoryUsed > 60; //memory meter only active (as an alert) if RAM usage is > 60%
                   bool showBattery = batteryUsed < 255; //Battery meter active only if battery is available

                   QString string;
                   if (showMemmory)
                       string += QString("MEM: %1%").arg(performanceMonitor->getMemmoryUsed());

                   if (showBattery)
                       string += QString(" BAT: %1%").arg(performanceMonitor->getBatteryUsed());

                   performanceMonitorLabel->setText(string);

                   performanceMonitorLabel->setVisible(showMemmory || showBattery);

               }

        lastPerformanceMonitorUpdate = now;
    }

    // prevent screen saver if user is playing
    if (mainController->isPlayingInNinjamRoom())
        screensaverBlocker->update();

    // update public server stream plot
    if (mainController->isPlayingRoomStream()) {
        auto roomID = mainController->getCurrentStreamingRoomID();
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
    ui.masterFader->setPeak(masterPeak.getLeftPeak(), masterPeak.getRightPeak(),
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

        ui.chatTabWidget->retranslateUi(); // translate the main chat tab title

        translatePublicChatCountryNames();
    }

    QMainWindow::changeEvent(ev);
}

void MainWindow::translatePublicChatCountryNames()
{
    auto publicChat = ui.chatTabWidget->getPublicChat();
    if (publicChat) {
        auto connectedUsers = publicChat->getConnectedUsers();

        for (const QString &userFullName : connectedUsers) {
            auto ip = ninjam::client::extractUserIP(userFullName);
            publicChat->updateUsersLocation(ip, mainController->getGeoLocation(ip));
        }
    }
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

    login::RoomInfo roomInfo(server, serverPort, 32, 32);
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

        connect(privateServerWindow.data(), &PrivateServerWindow::userConnectingInPrivateServer, [=](QString server, quint16 port){
            if (mainController)
                connectInPrivateServer(server, port, mainController->getUserName(), QString());
        });
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

    connect(dialog, &PreferencesDialog::jamDateFormatChanged, this, &MainWindow::setJamDirectoryDateFormat);

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

void MainWindow::setJamDirectoryDateFormat(const QString &newDateFormat)
{
    mainController->storeDirNameDateFormat(newDateFormat);
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
        AudioSlider::setPaintMaxPeakMarker(ui.actionShowMaxPeaks->isChecked());
    }
    else{
        if (action == ui.actionShowPeakAndRMS){
            AudioSlider::paintPeaksAndRms();
        }
        else if (action == ui.actionShowPeaksOnly){
            AudioSlider::paintPeaksOnly();
        }
        else{ // RMS only
            AudioSlider::paintRmsOnly();
        }
    }
    quint8 meterOption = 0; // rms + peaks
    if (AudioSlider::isPaintingPeaksOnly())
        meterOption = 1;
    else if (AudioSlider::isPaintingRmsOnly())
        meterOption = 2;

    mainController->storeMeteringSettings(AudioSlider::isPaintintMaxPeakMarker(), meterOption);
}

void MainWindow::updateMeteringMenu()
{
    ui.actionShowMaxPeaks->setChecked(AudioSlider::isPaintintMaxPeakMarker());
    bool showingPeakAndRms = AudioSlider::isPaintingRMS() && AudioSlider::isPaintingPeaks();
    if (showingPeakAndRms) {
        ui.actionShowPeakAndRMS->setChecked(true);
    }
    else{
        ui.actionShowPeaksOnly->setChecked(AudioSlider::isPaintingPeaks());
        ui.actionShowRmsOnly->setChecked(AudioSlider::isPaintingRMS());
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

void MainWindow::setVoiceChatStatus(int channelID, bool voiceChatStatus)
{
    mainController->setVoiceChatStatus(channelID, voiceChatStatus);

    if (mainController->isPlayingInNinjamRoom()) {
        auto ninjamService = mainController->getNinjamService();
        ninjamService->sendNewChannelsListToServer(getChannelsMetadata());
    }
}

bool MainWindow::isTransmiting(int channelID) const
{
    return mainController->isTransmiting(channelID);
}

void MainWindow::showJamtabaCurrentVersion()
{
    auto title = tr("About Jamtaba");
    auto text = tr("Jamtaba version is %1").arg(QApplication::applicationVersion());
    text += QString(" (%1 bits) \n\n").arg(QSysInfo::WordSize);
    text += QString("Qt: \t %1 \n").arg(qVersion());
    text += QString("FFMpeg:\t %1 \n").arg(av_version_info());
    text += QString("Vorbis:\t %1 \n").arg(vorbis_version_string());
    text += QString("MiniUpnP:\t %1 \n").arg(MINIUPNPC_VERSION);

    auto box = new QMessageBox();
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
    Q_ASSERT(ninjamWindow);

    auto chordsPanel = new ChordsPanel(ninjamWindow->getRoomName());

    connect(chordsPanel, &ChordsPanel::sendingChordsToChat, this, [=]()
    {
        if (mainController) { // just in case
            auto chordProgression = chordsPanel->getChordProgression();
            mainController->getNinjamController()->sendChatMessage(chordProgression.toString());
        }
    });

    connect(chordsPanel, &ChordsPanel::openingChordsDialog, ninjamWindow.data(), &NinjamRoomWindow::showChordProgressionDialog);

    connect(mainController->getNinjamController(), &NinjamController::intervalBeatChanged, chordsPanel, &ChordsPanel::setCurrentBeat);

    return chordsPanel;
}

void MainWindow::acceptChordProgression(const ChordProgression &progression)
{
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    if (progression.canBeUsed(currentBpi)) {

        // add the chord panel in a new tab
        showChordsPanel();

        auto chordsPanel = qobject_cast<ChordsPanel *>(ui.contentTabWidget->widget(2)); // get the chords tab
        if (!chordsPanel)
            return;

        bool needStrech = progression.getBeatsPerInterval() != currentBpi;
        if (needStrech)
            chordsPanel->setChords(progression.getStretchedVersion(currentBpi));
        else
            chordsPanel->setChords(progression);

        if (ninjamWindow) {
            auto ninjamPanel = ninjamWindow->getNinjamPanel();
            ninjamPanel->setLowContrastPaintInIntervalPanel(true);
        }

    } else {
        int measures = progression.getMeasures().size();
        QString msg = tr("These chords (%1 measures) can't be used in a %2 bpi interval!")
                      .arg(QString::number(measures))
                      .arg(QString::number(currentBpi));
        QMessageBox::warning(this, tr("Problem..."), msg);
    }
}

void MainWindow::showChordsPanel()
{
    if (!mainController || !mainController->isPlayingInNinjamRoom())
        return;

    if (!chordsPanelIsVisible()) { // chords tab is not created
        auto chordsPanel = createChordsPanel();
        auto tabTitle = tr("Chords");
        auto tabIndex = ui.contentTabWidget->addTab(chordsPanel, tabTitle);
        ui.contentTabWidget->setCurrentIndex(tabIndex);
    }
}

bool MainWindow::chordsPanelIsVisible() const
{
    return ui.contentTabWidget->count() >= 3;
}

ChordsPanel *MainWindow::getChordsPanel() const
{
    if (!chordsPanelIsVisible())
        return nullptr;

    return qobject_cast<ChordsPanel *>(ui.contentTabWidget->widget(2));
}

// ninjam controller events
void MainWindow::updateBpi(int bpi)
{
    if (!ninjamWindow)
        return;

    auto ninjamPanel = ninjamWindow->getNinjamPanel();

    if (!ninjamPanel)
        return;

    ninjamPanel->setBpi(bpi);

    auto chordsPanel = qobject_cast<ChordsPanel *>(ui.contentTabWidget->widget(2));
    if (chordsPanel) {
        bool bpiWasAccepted = chordsPanel->setBpi(bpi);
        if (!bpiWasAccepted)
            closeContentTab(2); // close chords tab
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
}

void MainWindow::setupWidgets()
{
    ui.masterFader->setOrientation(Qt::Horizontal);

    if (ui.allRoomsContent->layout())
        delete ui.allRoomsContent->layout();

    ui.allRoomsContent->setLayout(new QGridLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0, 0, 0, 0);

    ui.localTracksWidget->installEventFilter(this);

    QString lastUserName = mainController->getUserName();

    if (!lastUserName.isEmpty())
        ui.userNameLineEdit->setText(lastUserName);

    ui.masterTitleLabel->setVisible(true);

    ui.chatTabWidget->initialize(mainController, usersColorsPool.data());
    connect(ui.chatTabWidget, &ChatTabWidget::collapsedChanged, this, &MainWindow::chatCollapseChanged);

    // remember chat collapse status
    auto settings = mainController->getSettings();
    if (settings.isRememberingChatSection()) {
        ui.chatTabWidget->collapse(settings.isChatSectionCollapsed());
    }
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

    connect(ui.masterFader, &QSlider::valueChanged, this, &MainWindow::setMasterGain);

    connect(ui.menuLanguage, &QMenu::triggered, this, &MainWindow::setLanguage);

    connect(ui.userNameLineEdit, &UserNameLineEdit::editingFinished, this, &MainWindow::updateUserName);

    connect(mainController, &MainController::themeChanged, this, &MainWindow::handleThemeChanged);

    connect(mainController, &MainController::userBlockedInChat, this, &MainWindow::showFeedbackAboutBlockedUserInChat);
    connect(mainController, &MainController::userUnblockedInChat, this, &MainWindow::showFeedbackAboutUnblockedUserInChat);

    ui.contentTabWidget->installEventFilter(this);

}

void MainWindow::updateUserName()
{
    QString newUserName = ui.userNameLineEdit->text();
    mainController->setUserName(newUserName);

    mainChat->setUserName(mainController->getUserName());
}

void MainWindow::initializeMasterFader()
{
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

    connect(buttonCollapseChat, &QPushButton::clicked, ui.chatTabWidget, &ChatTabWidget::toggleCollapse);

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

    buttonCollapseChat->setChecked(!ui.chatTabWidget->isCollapsed());

    buttonCollapseLocalChannels->setChecked(!ui.localControlsCollapseButton->isChecked());

}

QString MainWindow::getChannelGroupName(int index) const
{
    if (index >= 0 && index < localGroupChannels.size()) {
        return localGroupChannels.at(index)->getChannelGroupName();
    }

    return QString();
}
