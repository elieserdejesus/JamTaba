#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"
#include "NinjamTrackGroupView.h"
#include "UsersColorsPool.h"
#include "ninjam/client/ServerInfo.h"
#include "ninjam/client/User.h"
#include "ninjam/client/UserChannel.h"
#include "ninjam/client/Service.h"
#include "audio/MetronomeTrackNode.h"
#include "audio/core/AudioDriver.h"
#include "audio/NinjamTrackNode.h"
#include "NinjamController.h"
#include "MainController.h"
#include "chords/ChordsPanel.h"
#include "chords/ChordProgression.h"
#include "chords/ChatChordsProgressionParser.h"
#include "NinjamPanel.h"
#include "chat/ChatPanel.h"
#include "chat/NinjamChatMessageParser.h"
#include "MainWindow.h"
#include "log/Logging.h"
#include "persistence/UsersDataCache.h"
#include "MetronomeUtils.h"

#include <QMessageBox>
#include <QRegExp>
#include <QLocale>
#include <cassert>
#include <QComboBox>
#include <QDebug>
#include <QDateTime>
#include <QToolButton>
#include <QButtonGroup>
#include <QTimer>

using controller::MainController;
using controller::NinjamController;
using login::RoomInfo;
using persistence::CacheEntry;

NinjamRoomWindow::NinjamRoomWindow(MainWindow *mainWindow, const RoomInfo &roomInfo, MainController *mainController) :
    QWidget(mainWindow),
    ui(new Ui::NinjamRoomWindow),
    mainWindow(mainWindow),
    mainController(mainController),
    ninjamPanel(nullptr),
    metronomePanel(nullptr),
    tracksLayout(TracksLayout::VerticalLayout),
    tracksSize(TracksSize::WIDE),
    metronomeFloatingWindow(nullptr),
    roomInfo(roomInfo),
    usersColorsPool(mainWindow->getUsersColorsPool()),
    chordProgressionDialog(new ChordProgressionCreationDialog(this))
{
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow::NinjamRoomWindow ctor";
    ui->setupUi(this);

    ui->licenceButton->setIcon(QIcon(QPixmap(":/images/licence.png")));

    ui->tracksPanel->layout()->setAlignment(Qt::AlignLeft); // tracks are left aligned

    ninjamPanel = createNinjamPanel();
    metronomePanel = createMetronomePanel();

    QString serverLicence = mainController->getNinjamService()->getCurrentServerLicence();
    ui->licenceButton->setVisible(!serverLicence.isEmpty());

    quint8 lastLayout = mainController->getLastTracksLayout();
    TracksLayout tracksLayout = TracksLayout::VerticalLayout;
    if (lastLayout == 1)
        tracksLayout = TracksLayout::HorizontalLayout;
    else if (lastLayout == 2)
            tracksLayout = TracksLayout::GridLayout;

    createLayoutButtons(tracksLayout);

    TracksSize lastTracksSize = mainController->isUsingNarrowedTracks() ? TracksSize::NARROW : TracksSize::WIDE;
    createTracksSizeButtons(lastTracksSize);

    setupSignals(mainController->getNinjamController());

    // remember the last tracks layout orientation and size (narrow or wide)
    setTracksLayout(tracksLayout);
    setTracksSize(lastTracksSize);

    translate();

    updateBpmBpiLabel();

    connect(mainController, &controller::MainController::themeChanged, this, &NinjamRoomWindow::updateStylesheet);
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow::NinjamRoomWindow done";

    setTintColor(mainWindow->getTintColor());
}

void NinjamRoomWindow::setTintColor(const QColor &color)
{
    Q_UNUSED(color);
}

void NinjamRoomWindow::setBpiComboPendingStatus(bool status)
{
    ninjamPanel->setBpiComboPendingStatus(status);
}

void NinjamRoomWindow::setBpmComboPendingStatus(bool status)
{
    ninjamPanel->setBpmComboPendingStatus(status);
}

void NinjamRoomWindow::updateStylesheet()
{
    if (metronomePanel)
        metronomePanel->updateStyleSheet();

    auto ninjamTracks = ui->tracksPanel->findChildren<NinjamTrackView *>();
    for (auto ninjamTrack : ninjamTracks) {
        ninjamTrack->updateStyleSheet();
    }
}

void NinjamRoomWindow::updateBpmBpiLabel()
{
    auto controller = mainController->getNinjamController();
    if (!controller)
        return;

    int bpi = controller->getCurrentBpi();
    int bpm = controller->getCurrentBpm();
    QString newText(QString::number(bpm) + " BPM   " + QString::number(bpi) + " BPI");
    ui->labelBpmBpi->setText(newText);
}

void NinjamRoomWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void NinjamRoomWindow::translate()
{
    ui->retranslateUi(this); // translate the fixed elements created in Qt ui designer

    // translate other elements
    horizontalLayoutButton->setToolTip(tr("Set tracks layout to horizontal"));
    verticalLayoutButton->setToolTip(tr("Set tracks layout to vertical"));
    gridLayoutButton->setToolTip(tr("Set tracks layout to grid"));
    wideButton->setToolTip(tr("Wide tracks"));
    narrowButton->setToolTip(tr("Narrow tracks"));

    updateUserNameLabel();
}

void NinjamRoomWindow::updateUserNameLabel()
{
    QString userName = mainController->getUserName();
    QString labelText = tr("Connected as %1").arg(userName);
    ui->labelUserName->setText(labelText);
}

void NinjamRoomWindow::createLayoutButtons(TracksLayout initialLayout)
{
    horizontalLayoutButton = new QToolButton();
    horizontalLayoutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    horizontalLayoutButton->setIcon(QIcon(":/images/horizontal_layout.png"));
    horizontalLayoutButton->setObjectName(QStringLiteral("buttonHorizontalLayout"));
    horizontalLayoutButton->setCheckable(true);

    verticalLayoutButton = new QToolButton();
    verticalLayoutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    verticalLayoutButton->setIcon(QIcon(":/images/vertical_layout.png"));
    verticalLayoutButton->setObjectName(QStringLiteral("buttonVerticalLayout"));
    verticalLayoutButton->setCheckable(true);

    gridLayoutButton = new QToolButton();
    gridLayoutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    gridLayoutButton->setIcon(QIcon(":/images/grid_layout.png"));
    gridLayoutButton->setObjectName(QStringLiteral("buttonGridLayout"));
    gridLayoutButton->setCheckable(true);

    if(initialLayout == TracksLayout::VerticalLayout)
        verticalLayoutButton->setChecked(true);
    else if(initialLayout == TracksLayout::HorizontalLayout)
        horizontalLayoutButton->setChecked(true);
    else if (initialLayout == TracksLayout::GridLayout)
        gridLayoutButton->setChecked(true);
    else
        qCritical() << "Invalid initial layout value " << static_cast<quint8>(initialLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(verticalLayoutButton);
    buttonsLayout->addWidget(horizontalLayoutButton);
    buttonsLayout->addWidget(gridLayoutButton);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(verticalLayoutButton);
    buttonGroup->addButton(horizontalLayoutButton);
    buttonGroup->addButton(gridLayoutButton);

    int licenceButtonIndex = ui->topLayout->indexOf(ui->licenceButton);
    ui->topLayout->insertLayout(licenceButtonIndex, buttonsLayout);

    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleTracksLayoutOrientation(QAbstractButton*)));
}

void NinjamRoomWindow::createTracksSizeButtons(TracksSize initialTracksSize)
{
    wideButton = new QToolButton();
    wideButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    wideButton->setIcon(QIcon(":/images/wide.png"));
    wideButton->setObjectName(QStringLiteral("wide"));
    wideButton->setCheckable(true);

    narrowButton = new QToolButton();
    narrowButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    narrowButton->setIcon(QIcon(":/images/narrow.png"));
    narrowButton->setObjectName(QStringLiteral("narrow"));
    narrowButton->setCheckable(true);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(wideButton);
    buttonsLayout->addWidget(narrowButton);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(wideButton);
    buttonGroup->addButton(narrowButton);

    if(initialTracksSize == TracksSize::NARROW)
        narrowButton->setChecked(true);
    else
        wideButton->setChecked(true);

    int licenceButtonIndex = ui->topLayout->indexOf(ui->licenceButton);
    ui->topLayout->insertLayout(licenceButtonIndex, buttonsLayout);

    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(toggleTracksSize(QAbstractButton*)));
}

void NinjamRoomWindow::toggleTracksLayoutOrientation(QAbstractButton* buttonClicked)
{
    if (buttonClicked == verticalLayoutButton) {
        setTracksLayout(TracksLayout::VerticalLayout);
    }
    else if (buttonClicked == horizontalLayoutButton) {
        setTracksLayout(TracksLayout::HorizontalLayout);
    }
    else if (buttonClicked == gridLayoutButton) {
        setTracksLayout(TracksLayout::GridLayout);
    }
}

void NinjamRoomWindow::toggleTracksSize(QAbstractButton *buttonClicked)
{
    TracksSize newTracksSize = buttonClicked == narrowButton ? TracksSize::NARROW : TracksSize::WIDE;
    setTracksSize(newTracksSize);
}

MetronomePanel *NinjamRoomWindow::createMetronomePanel()
{
    auto panel = new MetronomePanel(this);

    float initialMetronomeGain = mainController->getSettings().getMetronomeGain();
    float initialMetronomePan = mainController->getSettings().getMetronomePan();
    bool initialMetronomeMuteStatus = mainController->getSettings().getMetronomeMuteStatus();

    panel->setGainSliderValue(100 * initialMetronomeGain);
    panel->setPanSliderValue(4 * initialMetronomePan);
    panel->setMuteButtonStatus(initialMetronomeMuteStatus);

    connect(panel, &MetronomePanel::gainSliderChanged, this, &NinjamRoomWindow::setMetronomeFaderPosition);
    connect(panel, &MetronomePanel::panSliderChanged, this, &NinjamRoomWindow::setMetronomePanSliderPosition);
    connect(panel, &MetronomePanel::muteButtonClicked, this, &NinjamRoomWindow::toggleMetronomeMuteStatus);
    connect(panel, &MetronomePanel::soloButtonClicked, this, &NinjamRoomWindow::toggleMetronomeSoloStatus);
    connect(panel, &MetronomePanel::preferencesButtonClicked, this, &NinjamRoomWindow::showMetronomePreferences);
    connect(panel, &MetronomePanel::floatingWindowButtonToggled, this, &NinjamRoomWindow::showMetronomeFloatingWindow);

    return panel;
}

NinjamPanel *NinjamRoomWindow::createNinjamPanel()
{
    TextEditorModifier *bpiComboModifier = mainWindow ? mainWindow->createTextEditorModifier() : nullptr;
    TextEditorModifier *bpmComboModifier = mainWindow ? mainWindow->createTextEditorModifier() : nullptr;
    TextEditorModifier *accentBeatsModifier = mainWindow ? mainWindow->createTextEditorModifier() : nullptr;
    NinjamPanel *panel = new NinjamPanel(bpiComboModifier, bpmComboModifier, accentBeatsModifier, this);

    panel->setIntervalShape(mainController->getSettings().getIntervalProgressShape());
    panel->setAccentBeatsReadOnly(true);
    panel->setAccentBeatsVisible(false);

    connect(panel, &NinjamPanel::bpiComboActivated, this, &NinjamRoomWindow::setNewBpi);
    connect(panel, &NinjamPanel::bpmComboActivated, this, &NinjamRoomWindow::setNewBpm);
    connect(panel, &NinjamPanel::accentsComboChanged, this, &NinjamRoomWindow::handleAccentBeatsComboChange);
    connect(panel, &NinjamPanel::accentsBeatsChanged, this, &NinjamRoomWindow::handleCustomAccentBeatsChange);
    connect(panel, &NinjamPanel::midiSyncChanged, this, &NinjamRoomWindow::handleMidiSyncChange);

    return panel;
}

void NinjamRoomWindow::showMetronomeFloatingWindow(bool show)
{
    if (!ninjamPanel || !metronomePanel)
        return;

    if (show){
        if (!metronomeFloatingWindow) {

            auto paintShape = static_cast<IntervalProgressDisplay::PaintShape>(ninjamPanel->getIntervalShape());
            int beatsPerInterval = ninjamPanel->getBpi();
            QList<int> accentBeats = ninjamPanel->getAccentBeats();
            bool showingAccents = ninjamPanel->isShowingAccents();
            metronomeFloatingWindow = new IntervalProgressWindow(nullptr, paintShape , beatsPerInterval, accentBeats, showingAccents);
            connect(metronomeFloatingWindow, &IntervalProgressWindow::windowClosed, this, &NinjamRoomWindow::deleteFloatingWindow);

            ninjamPanel->setMetronomeFloatingWindow(metronomeFloatingWindow);
        }

        metronomeFloatingWindow->move(10, 10); // top left
        metronomeFloatingWindow->setVisible(true);
        metronomeFloatingWindow->raise();

    }
    else{
        if (metronomeFloatingWindow) {
            metronomeFloatingWindow->setVisible(false);
            deleteFloatingWindow();
        }
    }

    metronomePanel->setFloatingWindowButtonChecked(show);
}

void NinjamRoomWindow::showMetronomePreferences()
{
    Q_ASSERT(mainWindow);

    mainWindow->showMetronomePreferencesDialog();
}

void NinjamRoomWindow::updateGeoLocations()
{
    foreach (NinjamTrackGroupView *trackGroup, trackGroups)
        trackGroup->updateGeoLocation();
}

void NinjamRoomWindow::setMetronomePanSliderPosition(int value)
{
    if (!metronomePanel)
        return;

    float sliderValue = value/(float)metronomePanel->getPanSliderMaximumValue();
    mainController->setTrackPan(controller::NinjamController::METRONOME_TRACK_ID, sliderValue);
}

void NinjamRoomWindow::setMetronomeFaderPosition(int value)
{
    mainController->setTrackGain(controller::NinjamController::METRONOME_TRACK_ID, value/100.0);
}

void NinjamRoomWindow::toggleMetronomeMuteStatus()
{
    mainController->setTrackMute(controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsMuted(
                                     controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::toggleMetronomeSoloStatus()
{
    mainController->setTrackSolo(controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsSoloed(
                                     controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::resetBpiComboBox()
{
    auto ninjamController = mainController->getNinjamController();
    ninjamPanel->setBpiComboPendingStatus(false);
    ninjamPanel->setBpiComboText(QString::number(ninjamController->getCurrentBpi()));
}

void NinjamRoomWindow::resetBpmComboBox()
{
    auto ninjamController = mainController->getNinjamController();
    ninjamPanel->setBpmComboPendingStatus(false);
    ninjamPanel->setBpmComboText(QString::number(ninjamController->getCurrentBpm()));
}

void NinjamRoomWindow::updatePeaks()
{
    for (auto view : trackGroups) {
        if (view)
            view->updateGuiElements();
    }

    if (!metronomePanel)
        return;

    auto metronomePeak = mainController->getTrackPeak(controller::NinjamController::METRONOME_TRACK_ID);

    metronomePanel->setMetronomePeaks(metronomePeak.getLeftPeak(),
                                   metronomePeak.getRightPeak(),
                                   metronomePeak.getLeftRMS(),
                                   metronomePeak.getRightRMS());
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NinjamRoomWindow::hideIntervalDownloadingProgressBar(long trackID)
{
    NinjamTrackView *trackView = getTrackViewByID(trackID);
    if (trackView)
        trackView->finishCurrentDownload();
}

void NinjamRoomWindow::updateIntervalDownloadingProgressBar(long trackID)
{
    NinjamTrackNode *node = dynamic_cast<NinjamTrackNode *>(mainController->getTrackNode(trackID));
    NinjamTrackView *trackView = getTrackViewByID(trackID);
    if (node && trackView) {
        bool waitingToPlayTheFirstInterval = !node->isPlaying();
        if (waitingToPlayTheFirstInterval)
            trackView->incrementDownloadedChunks();
    }
}

void NinjamRoomWindow::setChannelXmitStatus(long channelID, bool transmiting)
{
    NinjamTrackView *trackView = getTrackViewByID(channelID);
    if (trackView)
        trackView->setActivatedStatus(!transmiting);
}

void NinjamRoomWindow::removeChannel(const User &user, const UserChannel &channel, long channelID)
{
    auto group = trackGroups[user.getFullName()];
    if (group) {
        if (group->getTracksCount() == 1) { // removing the last track, the group is removed too
            trackGroups.remove(user.getFullName());
            ui->tracksPanel->layout()->removeWidget(group);
            group->deleteLater();
        } else { // remove one subchannel
            auto trackView = BaseTrackView::getTrackViewByID(channelID);
            if (trackView)
                group->removeTrackView(trackView);
        }

        qCDebug(jtNinjamGUI) << "channel removed:" << channel.getName();

        updateTracksSizeButtons();

        reAddTrackGroups(); // update the gridlayout to avoid empty cells
    }
}

NinjamTrackView *NinjamRoomWindow::getTrackViewByID(long trackID)
{
    return dynamic_cast<NinjamTrackView *>(NinjamTrackView::getTrackViewByID(trackID));
}

void NinjamRoomWindow::changeChannel(const User &, const UserChannel &channel, long channelID)
{
    // invoked when channel name or flags (intervalic or voice chat) are changed

    qCDebug(jtNinjamGUI) << "channel name or flags changed:" << channel.getName();
    auto trackView = getTrackViewByID(channelID);
    if (trackView) {
        trackView->setChannelName(channel.getName());
        trackView->setChannelMode(channel.isVoiceChatChannel() ? NinjamTrackNode::VoiceChat : NinjamTrackNode::Intervalic);
    }
}

quint8 NinjamRoomWindow::getGridLayoutMaxCollumns() const
{
    static const uint minCollumns = 2;
    static const uint maxCollumns = 3;

    return qMax(minCollumns, qMin(width()/NinjamTrackGroupView::MAX_WIDTH_IN_GRID_LAYOUT, maxCollumns));
}

void NinjamRoomWindow::addTrack(NinjamTrackGroupView *track)
{
    int row = 0;
    int collumn = 0;

    if (tracksLayout == TracksLayout::VerticalLayout) {
        row = 0;
        collumn = ui->tracksLayout->columnCount();
    } else if (tracksLayout == TracksLayout::HorizontalLayout) {
        row = ui->tracksLayout->rowCount();
        collumn = 0;
    }
    else if (tracksLayout == TracksLayout::GridLayout) {

        quint8 collumns = getGridLayoutMaxCollumns();
        if (collumns == 0) // avoid divide by zero
            collumns = 2;

        row = ui->tracksLayout->count() / collumns;
        collumn = ui->tracksLayout->count() % collumns;
    }

    Qt::Alignment alignment = tracksLayout == TracksLayout::VerticalLayout ? Qt::AlignLeft : Qt::AlignTop;
    auto vPolicy = tracksLayout == TracksLayout::VerticalLayout ? QSizePolicy::Expanding : QSizePolicy::Preferred;
    auto hPolicy = tracksLayout == TracksLayout::HorizontalLayout ? QSizePolicy::MinimumExpanding : QSizePolicy::Preferred;
    track->setSizePolicy(QSizePolicy(hPolicy, vPolicy));

    auto colSpan = 1;
    auto rowSpan = 1;
    ui->tracksLayout->addWidget(track, row, collumn, rowSpan, colSpan, alignment);

}

void NinjamRoomWindow::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);

    if (tracksLayout == TracksLayout::GridLayout) {
        quint8 preferredCollumns = getGridLayoutMaxCollumns();
        if (preferredCollumns != ui->tracksLayout->columnCount()) {
            reAddTrackGroups();
        }
    }

}

bool cameraSorter(NinjamTrackGroupView *track1, NinjamTrackGroupView *)
{
     return track1 && track1->isShowingVideo();
}

void NinjamRoomWindow::reAddTrackGroups()
{
    for (auto trackGroup : trackGroups.values()) // remove all tracks from layout
        ui->tracksLayout->removeWidget(trackGroup);

    auto itemsToAdd = trackGroups.values();

    if (tracksLayout == TracksLayout::GridLayout) { // if layout is Grid reorder trackGroups to show active cams first
        qSort(itemsToAdd.begin(), itemsToAdd.end(), cameraSorter);
    }

    for (auto trackGroup : itemsToAdd)
        addTrack(trackGroup);

    adjustTracksPanelSizePolicy();
}

void NinjamRoomWindow::addChannel(const User &user, const UserChannel &channel, long channelID)
{
    qCDebug(jtNinjamGUI) << "channel added - creating channel view:" << user.getFullName() << " "
                         << channel.getName();

    // remembering user track controls (gain, mute, pan, boost)
   auto cache = mainController->getUsersDataCache();

    // a empty/default cacheEntry is returned if the user is not cached
    CacheEntry cacheEntry = cache->getUserCacheEntry(user.getIp(), user.getName(), static_cast<quint8>(channel.getIndex()));

    if (!trackGroups.contains(user.getFullName())) { // first channel from this user?
        auto channelName = channel.getName();
        auto channelMode = channel.isVoiceChatChannel() ? NinjamTrackNode::VoiceChat : NinjamTrackNode::Intervalic;
        auto userColor = usersColorsPool->get(user.getName()); // the user channel and your chat messages are painted with same color
        auto trackGroupView = new NinjamTrackGroupView(mainController, channelID, channelName,
                                                       channelMode, userColor, cacheEntry);
        trackGroupView->setTracksLayout(tracksLayout);
        trackGroupView->setNarrowStatus(tracksSize == TracksSize::NARROW);
        addTrack(trackGroupView);
        trackGroups.insert(user.getFullName(), trackGroupView);
        trackGroupView->setEstimatedChunksPerInterval(calculateEstimatedChunksPerInterval());

        connect(trackGroupView, &NinjamTrackGroupView::createPrivateChat, mainWindow, &MainWindow::addPrivateChat);

    } else { // the second, or third channel from same user, group with other channels
        auto trackGroup = trackGroups[user.getFullName()];
        if (trackGroup) {
            auto ninjamTrackView = trackGroup->addTrackView(channelID);
            ninjamTrackView->setChannelName(channel.getName());
            ninjamTrackView->setChannelMode(channel.isVoiceChatChannel() ? NinjamTrackNode::VoiceChat : NinjamTrackNode::Intervalic);
            ninjamTrackView->setInitialValues(cacheEntry);
            ninjamTrackView->setEstimatedChunksPerInterval(calculateEstimatedChunksPerInterval());
            ninjamTrackView->setActivatedStatus(true); /** disabled/grayed until receive the first bytes. When the first bytes
            are downloaded the 'on_channelXmitChanged' slot is executed and this track is enabled.*/
        }
    }

    // bind the new track view with ninjam channel data (user full name and channel index). These ninjam data is necessary to send receive on/off messages to server when user click in 'receive' button
    auto groupView = trackGroups[user.getFullName()];
    if (groupView != nullptr) {
        auto trackViews = groupView->getTracks<NinjamTrackView*>();
        if (!trackViews.isEmpty())
            trackViews.last()->setNinjamChannelData(user.getFullName(), channel.getIndex());
    }

    qCDebug(jtNinjamGUI) << "channel view created";

    updateTracksSizeButtons();
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int NinjamRoomWindow::calculateEstimatedChunksPerInterval() const
{
    auto ninjamController = mainController->getNinjamController();
    if(ninjamController){
        int bpi = ninjamController->getCurrentBpi();
        int bpm = ninjamController->getCurrentBpm();
        if(bpm > 0){
            return (60000/bpm * bpi) / 320;

            /** 320 is just a 'almost magical empyrical (non exact)' value estimated observing how many chunks are downloaded from a remote stereo audio stream (48 KHz).
                To compute exactly how many chunks per interval we need some infos like: remote sample rate, remote is mono or stereo, bpm, bpi, and the
                quality used in the remove encoder (more quality need more bytes, and more chunks). Some of these infos are very trick to obtain, so
                I'm just using a 'good enough guess' approach. Feel free to improve!
            */
        }
    }
    return 0;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NinjamRoomWindow::~NinjamRoomWindow()
{
    auto ninjamController = mainController->getNinjamController();
    if (ninjamController) {
        disconnect(ninjamController); // disconnect signal/slots
    }

    if (ninjamPanel) {
        ninjamPanel->deleteLater();
        ninjamPanel = nullptr;
    }

    if (metronomePanel) {
        metronomePanel->deleteLater();
        metronomePanel = nullptr;
    }

    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->deleteLater();
        metronomeFloatingWindow = nullptr;
    }

    delete ui;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NinjamRoomWindow::showServerLicence()
{
    static QRegExp ccLink("(http://creativecommons.org/licenses/\\S+/)");
    static QRegExp brPattern("(\\r\\n|\\r|\\n)");
    QString locale = QLocale::system().name();
    QString anchorTag = QStringLiteral("<a href=\"\\1deed.%1\">\\1</a>").arg(locale);

    QString licence = mainController->getNinjamService()->getCurrentServerLicence();
    QMessageBox *msgBox = new QMessageBox();

    msgBox->setWindowIcon(mainWindow->windowIcon());
    msgBox->setTextFormat(Qt::RichText);
    msgBox->setText(licence
              .replace("<", "&lt;")
              .replace(brPattern, "<br>")
              .replace(ccLink, anchorTag));
    msgBox->setWindowTitle(tr("Server Licence"));
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowModality(Qt::WindowModal);

    // hack to set minimum width in QMessageBox
    QSpacerItem *horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum,
                                                    QSizePolicy::Expanding);
    QGridLayout *layout = static_cast<QGridLayout *>(msgBox->layout());
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox->raise();
    
    msgBox->show();
    msgBox->exec();
}

// ----------
void NinjamRoomWindow::handleAccentBeatsComboChange(int index)
{
    Q_UNUSED(index)
    int accentBeatsCb = ninjamPanel->getAccentBeatsComboValue();

    if (accentBeatsCb == -1) {
        ninjamPanel->setAccentBeatsReadOnly(false);
        ninjamPanel->setAccentBeatsVisible(true);
        // do nothing else until the Ninjam Panel UI works out the accent beats text
    } else {
        ninjamPanel->setAccentBeatsReadOnly(true);
        ninjamPanel->setAccentBeatsVisible(false);
        int currentBpi = mainController->getNinjamController()->getCurrentBpi();
        mainController->getNinjamController()->setMetronomeBeatsPerAccent(accentBeatsCb, currentBpi);
    }
}

void NinjamRoomWindow::handleCustomAccentBeatsChange(const QList<int> &accentBeats)
{
    mainController->getNinjamController()->setMetronomeAccentBeats(accentBeats);
}

void NinjamRoomWindow::handleMidiSyncChange(const bool syncOn)
{
    mainController->getNinjamController()->setSyncEnabled(syncOn);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::setNewBpi(const QString &newText)
{
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    int newBpi = newText.toInt();
    if (newBpi == currentBpi)
        return;

    mainController->getNinjamController()->voteBpi(newBpi);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::setNewBpm(const QString &newText)
{
    int currentBpm = mainController->getNinjamController()->getCurrentBpm();
    int newBpm = newText.toInt();
    if (newBpm == currentBpm)
        return;

    mainController->getNinjamController()->voteBpm(newBpm);
}

void NinjamRoomWindow::setupSignals(controller::NinjamController* ninjamController)
{
    if(!ninjamController)
        return;

    qCDebug(jtNinjamGUI) << "connecting signals in ninjamController...";
    connect(ninjamController, &NinjamController::channelAdded, this, &NinjamRoomWindow::addChannel);
    connect(ninjamController, &NinjamController::channelRemoved, this, &NinjamRoomWindow::removeChannel);

    connect(ninjamController, &NinjamController::channelChanged, this, &NinjamRoomWindow::changeChannel);

    connect(ninjamController, &NinjamController::channelAudioChunkDownloaded, this, &NinjamRoomWindow::updateIntervalDownloadingProgressBar);

    connect(ninjamController, &NinjamController::channelAudioFullyDownloaded, this, &NinjamRoomWindow::hideIntervalDownloadingProgressBar);

    connect(ninjamController, &NinjamController::channelXmitChanged, this, &NinjamRoomWindow::setChannelXmitStatus);

    connect(ninjamController, &NinjamController::currentBpiChanged, this, &NinjamRoomWindow::handleBpiChanges);
    connect(ninjamController, &NinjamController::currentBpmChanged, this, &NinjamRoomWindow::handleBpmChanges);

    connect(ui->licenceButton, &QPushButton::clicked, this, &NinjamRoomWindow::showServerLicence);

    connect(ninjamPanel, &NinjamPanel::intervalShapeChanged, this, &NinjamRoomWindow::setNewIntervalShape);

    connect(mainController->getNinjamService(), &ninjam::client::Service::videoIntervalCompleted, this, &NinjamRoomWindow::setVideoInterval);

    connect(ui->chordsButton, &QPushButton::clicked, [=](){

        auto chordsPanel = mainWindow->getChordsPanel();
        auto currentProgression = ChordProgression();
        if (chordsPanel)
            currentProgression = chordsPanel->getChordProgression();

        showChordProgressionDialog(currentProgression);
    });
}

void NinjamRoomWindow::showChordProgressionDialog(const ChordProgression &currentProgression)
{
    if (chordProgressionDialog) {
        auto currentBpi = ninjamPanel->getBpi();
        chordProgressionDialog->show(currentBpi, currentProgression);
    }
}

void NinjamRoomWindow::setVideoInterval(const User &user, const QByteArray &encodedVideoData)
{
    auto group = trackGroups[user.getFullName()];
    if (group) {
        group->addVideoInterval(encodedVideoData);
    }
}

void NinjamRoomWindow::handleBpiChanges()
{
    setEstimatatedChunksPerIntervalInAllTracks();
    updateBpmBpiLabel();
    resetBpiComboBox();
}

void NinjamRoomWindow::handleBpmChanges()
{
    setEstimatatedChunksPerIntervalInAllTracks();
    updateBpmBpiLabel();
    resetBpmComboBox();
}

void NinjamRoomWindow::setNewIntervalShape(int newShape)
{
    mainController->storeIntervalProgressShape(newShape);
}

void NinjamRoomWindow::setEstimatatedChunksPerIntervalInAllTracks()
{
    int estimatedChunksPerInterval = calculateEstimatedChunksPerInterval();
    foreach (NinjamTrackGroupView *trackGroup, trackGroups.values()) {
        trackGroup->setEstimatedChunksPerInterval(estimatedChunksPerInterval);
    }
}

void NinjamRoomWindow::setTracksSize(TracksSize newTracksSize)
{
    if (newTracksSize == tracksSize)
        return;

    tracksSize = newTracksSize;

    bool usingNarrowedTracks = tracksSize == TracksSize::NARROW;
    for (auto trackGroupView : trackGroups) {
        trackGroupView->setNarrowStatus(usingNarrowedTracks);
    }

    mainController->storeTracksSize(usingNarrowedTracks);

    updateGeometry();
}

void NinjamRoomWindow::adjustTracksPanelSizePolicy()
{
    if (tracksLayout == TracksLayout::HorizontalLayout) {
        ui->tracksPanel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    }
    else if (tracksLayout == TracksLayout::VerticalLayout) {
        ui->tracksPanel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    }
    else if (tracksLayout == TracksLayout::GridLayout) {
        ui->tracksPanel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    }
    else {
        qCritical() << "Can't adjust for layout value" << static_cast<int>(tracksLayout);
    }

    updateGeometry();
}

void NinjamRoomWindow::setTracksLayout(TracksLayout newLayout)
{
    if(newLayout == tracksLayout)
        return;

    tracksLayout = newLayout;
    for (auto group : trackGroups) {
        group->setTracksLayout(newLayout);
    }

    reAddTrackGroups();

    mainController->storeTracksLayoutOrientation(static_cast<quint8>(newLayout));
}

void NinjamRoomWindow::updateTracksSizeButtons()
{
    //tracks size buttons are disabled if all users are using 2 tracks
    bool enableButtons = false;
    for (auto trackGroup : trackGroups) {
        if (trackGroup->getTracksCount() < 2){
            enableButtons = true;
            break;
        }
    }

    narrowButton->setEnabled(enableButtons);
    wideButton->setEnabled(enableButtons);
}


bool NinjamRoomWindow::metronomeFloatingWindowIsVisible() const
{
    return metronomeFloatingWindow && metronomeFloatingWindow->isVisible();
}

void NinjamRoomWindow::closeMetronomeFloatingWindow()
{
    if (metronomeFloatingWindowIsVisible()) {
        metronomeFloatingWindow->setVisible(false);
    }
}

void NinjamRoomWindow::deleteFloatingWindow()
{
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->deleteLater();
        metronomeFloatingWindow = nullptr;

        if (metronomePanel)
            metronomePanel->setFloatingWindowButtonChecked(false);

        if (ninjamPanel)
            ninjamPanel->setMetronomeFloatingWindow(nullptr);
    }
}
